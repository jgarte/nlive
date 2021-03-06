#ifndef NLIVE_AUDIO_STORAGE_ITEM_H_
#define NLIVE_AUDIO_STORAGE_ITEM_H_

#include "base/common/memory.h"

#include "model/resource/video_resource.h"
#include "model/resource/audio_resource.h"
#include "model/storage/storage_item.h"

namespace nlive {

class AudioStorageItem : public StorageItem {

private:
  AudioStorageItem() = default;
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & boost::serialization::base_object<StorageItem>(*this);
    ar & audio_resource_;
  }

  sptr<AudioResource> audio_resource_;

public:
  static const std::string TYPE;

  AudioStorageItem(
    sptr<StorageItem> parent,
    QString name,
    sptr<AudioResource> audio_resource);

  sptr<Clip> cliperize(sptr<Sequence> sequence) override;

  sptr<AudioResource> audio_resource();

  inline bool is_directory() const { return false; }

};

}

#endif