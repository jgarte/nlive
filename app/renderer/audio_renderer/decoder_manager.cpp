#include "renderer/audio_renderer/decoder_manager.h"

#include <QDebug>

namespace nlive {

namespace audio_renderer {

DecoderManager::DecoderManager() {

}

sptr<AudioDecoderRef> DecoderManager::acquireDecoder(
    sptr<RawAudioResource> resource, int referer) {
  sptr<AudioDecoderRef> ret = nullptr;
  int min_freq = 1;
  int idx = -1;
  int i = 0;
  for (auto decoder_ref : decoder_refs_) {
    if (decoder_ref->resource() == resource) {
      if (decoder_ref->referer() == referer) {
        ret = decoder_ref;
        idx = i;
        break;
      }
      if (decoder_ref->freq() < 0) {
        if (decoder_ref->freq() < min_freq) {
          ret = decoder_ref;
          min_freq = decoder_ref->freq();
          idx = i;
        }
      }
    }
    i++;
  }
  if (ret != nullptr) {
    decoder_refs_.erase(decoder_refs_.begin() + idx);
    return ret;
  }
  auto decoder = sptr<AudioDecoder>(new AudioDecoder(resource));
  ret = sptr<AudioDecoderRef>(new AudioDecoderRef(decoder, resource, referer));
  return ret;
}

void DecoderManager::releaseDecoder(
    sptr<AudioDecoderRef> decoder_ref) {
  decoder_refs_.push_back(decoder_ref);
}

}

}