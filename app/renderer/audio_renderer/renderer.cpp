#include "renderer/audio_renderer/renderer.h"

#include <QDebug>

namespace nlive {

namespace audio_renderer {

Renderer::Renderer(int sample_rate, int kernel_size, int kernel_length_per_slot,
    int slot_length) :
  sample_rate_(sample_rate) {
  render_state_ = std::make_shared<RenderState>(
    sample_rate, kernel_size, kernel_length_per_slot, slot_length);
  render_io_ = std::make_shared<RenderIO>(this, render_state_);
  render_context_ = QSharedPointer<RenderContext>(
    new RenderContext(render_state_->sample_rate(), render_state_->buffer()->bytes_per_slot()));
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
    int writing_index;
    if (producer_index < consumer_index)
      writing_index = consumer_index + 1;
    else writing_index = producer_index;

    std::unique_lock<std::mutex> state_lock(state_mutex_);
    writing_index_ = writing_index;
    emit onRenderRequest(writing_index);
    // Wait until render data arrived
    state_ = State::WAITING_DATA;
    state_cv_.wait(state_lock);
    if (state_ == State::RESET) continue;
    // Is this check statement redundant?
    assert(state_ == State::DATA_AVAILABLE);
    QSharedPointer<CommandBuffer> rcb = requested_command_buffer_;
    for (auto command : rcb->commands()) {
      command->render(render_context_);
    }
    state_lock.unlock();

    auto& slot_mutex = render_state_->slot_mutex_at(writing_index);
    auto buffer = render_state_->buffer();
    slot_mutex.lock();
    buffer->copyFrom(writing_index, render_context_->data());
    state_lock.lock();
    render_state_->setProducerIndex(writing_index + 1);
    state_lock.unlock();
    slot_mutex.unlock();
    render_context_->clearData();
  }
}

void Renderer::reset() {
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    auto& render_state_mutex = render_state_->state_mutex();
    render_state_mutex.lock();
    render_state_->reset();
    render_state_mutex.unlock();
    state_ = State::RESET;
}

void Renderer::sendRenderCommandBuffer(QSharedPointer<CommandBuffer> command_buffer, int index) {
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    if (writing_index_ != index) return;
    requested_command_buffer_ = command_buffer;
    state_ = State::DATA_AVAILABLE;
    state_cv_.notify_one();
}

}

}