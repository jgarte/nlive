#ifndef _NLIVE_RESOURCE_STORAGE_ITEM_H_
#define _NLIVE_RESOURCE_STORAGE_ITEM_H_

#include <QSharedPointer>

#include "model/resource/resource.h"
#include "model/storage/storage_item.h"

namespace nlive {

class ResourceStorageItem : public StorageItem {

private:
  QSharedPointer<Resource> resource_;

public:
  ResourceStorageItem(
    QString type,
    QSharedPointer<StorageItem> parent,
    QSharedPointer<Resource> resource);

  QSharedPointer<Resource> resource();

  inline bool is_directory() const { return false; }

};

}

#endif