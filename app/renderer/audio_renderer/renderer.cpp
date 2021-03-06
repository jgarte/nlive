#include "renderer/audio_renderer/renderer.h"

#include <QDebug>

namespace nlive {

namespace audio_renderer {

Renderer::Renderer(int64_t ch_layout, AVSampleFormat sample_fmt, int sample_rate,
    int samples_per_channel, int kernels_per_slot, int slot_length) :
  sample_rate_(sample_rate), samples_per_channel_(samples_per_channel) {
  int nb_channels = av_get_channel_layout_nb_channels(ch_layout);
  int bytes_per_sample = av_get_bytes_per_sample(sample_fmt);
  render_state_ = std::make_shared<RenderState>(
    nb_channels, bytes_per_sample, sample_rate, samples_per_channel,
    kernels_per_slot, slot_length);
  render_io_ = std::make_shared<RenderIO>(nullptr, render_state_);
  render_context_ = sptr<RenderContext>(
    new RenderContext(ch_layout, sample_fmt, sample_rate,
        samples_per_channel));
  requested_command_buffer_ = nullptr;
  requested_burst_command_buffer_ = nullptr;
}

Renderer::~Renderer() {
  close();
  wait();
}

void Renderer::run() {
  render_io_->start();
  while (true) {
    auto& render_state_mutex = render_state_->state_mutex();
    auto& state_cv = render_state_->state_cv();
    std::unique_lock<std::mutex> render_state_lock(render_state_mutex);
    // Wait until Producer slot is available
    render_state_->setProducerWaitFlag(true);
    state_cv.wait(render_state_lock, [this]{
      int producer_index = render_state_->producer_index();
      int consumer_index = render_state_->consumer_index();
      return producer_index - consumer_index < render_state_->slot_length();
    });
    // Producer slot is available
    int producer_index = render_state_->producer_index();
    int consumer_index = render_state_->consumer_index();
    render_state_->setProducerWaitFlag(false);
    render_state_lock.unlock();

    std::unique_lock<std::mutex> state_lock(state_mutex_);
    if (state_ == State::CLOSED) break;
    int writing_index;
    int next_producer_index;
    if (requested_burst_command_buffer_ != nullptr) {
      writing_index = consumer_index;
      sptr<CommandBuffer> rcb = requested_burst_command_buffer_;
      requested_burst_command_buffer_ = nullptr;
      state_lock.unlock();
      for (auto command : rcb->commands()) {
        command->render(render_context_);
      }
      next_producer_index = std::max(producer_index, writing_index + 1);
    }

    else {
      if (producer_index < consumer_index)
        writing_index = consumer_index + 1;
      else writing_index = producer_index;

      writing_index_ = writing_index;
      int64_t start_frame = calculateFrameByIndex(writing_index);
      int64_t end_frame = calculateFrameByIndex(writing_index + 1);
      emit onRenderRequest(writing_index, start_frame, end_frame);
      // Wait until render data arrived
      state_ = State::WAITING_DATA;
      state_cv_.wait(state_lock);
      if (state_ == State::CLOSED) break;
      if (state_ == State::RESET) {
        state_ = State::IDLE;
        continue;
      }
      // Is this check statement redundant?
      assert(state_ == State::DATA_AVAILABLE);
      sptr<CommandBuffer> rcb;
      if (requested_burst_command_buffer_) continue;
      else rcb = requested_command_buffer_;
      state_lock.unlock();
      for (auto command : rcb->commands()) {
        command->render(render_context_);
      }
      next_producer_index = writing_index + 1;
    }

    auto& slot_mutex = render_state_->slot_mutex_at(writing_index);
    auto buffer = render_state_->buffer();
    // TODO : FIXME :
    // This seems really inefficient. So many locks.
    slot_mutex.lock();
    buffer->copyFrom(writing_index, render_context_->data());
    slot_mutex.unlock();
    state_lock.lock();
    if (state_ == State::CLOSED) break;
    if (state_ == State::RESET) {
      state_ = State::IDLE;
      continue;
    }
    render_state_lock.lock();
    render_state_->setProducerIndex(next_producer_index);
    render_state_lock.unlock();
    state_lock.unlock();
    render_context_->clearData();
  }
  qDebug() << "[AudioRenderer] End of execution.";
}

void Renderer::reset() {
  std::unique_lock<std::mutex> state_lock(state_mutex_);
  auto& render_state_mutex = render_state_->state_mutex();
  render_state_mutex.lock();
  render_state_->reset();
  render_state_mutex.unlock();
  requested_burst_command_buffer_ = nullptr;
  requested_command_buffer_ = nullptr;
  state_ = State::RESET;
  state_cv_.notify_one();
}

void Renderer::close() {
  std::unique_lock<std::mutex> state_lock(state_mutex_);
  state_ = State::CLOSED;
  qDebug() << "[AudioRenderer] close!";
  state_cv_.notify_all();
  render_io_->exit();
}

void Renderer::sendBurstRenderCommandBuffer(sptr<CommandBuffer> command_buffer) {
  std::unique_lock<std::mutex> state_lock(state_mutex_);
  requested_burst_command_buffer_ = command_buffer;
  state_ = State::DATA_AVAILABLE;
  state_cv_.notify_one();
}

void Renderer::sendRenderCommandBuffer(sptr<CommandBuffer> command_buffer, int index) {
  std::unique_lock<std::mutex> state_lock(state_mutex_);
  if (writing_index_ != index) return;
  requested_command_buffer_ = command_buffer;
  state_ = State::DATA_AVAILABLE;
  state_cv_.notify_one();
}

int64_t Renderer::calculateFrameByIndex(int index) const {
  return samples_per_channel_ * index;
}

}

}