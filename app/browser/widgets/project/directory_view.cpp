#include "browser/widgets/project/directory_view.h"

#include "platform/logger/logger.h"
#include "platform/theme/themeservice.h"
#include "platform/resource/resource_service.h"
#include "model/storage/storage_directory.h"
#include "model/storage/storage_item.h"
#include "model/storage/resource_storage_item.h"
#include "browser/widgets/project/storage_item_view.h"
#include "browser/widgets/project/storage_item_view_factory.h"
#include "browser/services/import/import_service.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QThread>
#include <QPainter>
#include <QTimer>
#include <QFileInfo>
#include <QDebug>

namespace nlive {

namespace project_widget {

DirectoryView::DirectoryView(
  QWidget* parent,
  sptr<StorageDirectory> storage_directory,
  sptr<IThemeService> theme_service,
  sptr<IImportService> import_service) :
  theme_service_(theme_service),
  import_service_(import_service),
  Div(parent), storage_directory_(storage_directory) {
  setAcceptDrops(true);
  grid_layout_ = new GridLayout(this);
  grid_layout_->setBorder(Div::ALL, 1, theme_service_->getTheme().surfaceBrightColor());

  int i = 0;
  for (auto item : storage_directory->items())
    addStorageItemView(item, i ++);
  storage_directory->onDidAddItem.connect(SIG2_TRACK(sig2_t<void (sptr<StorageItem> item, int index)>::slot_type(
    [this](sptr<StorageItem> item, int index) {
    addStorageItemView(item, index);
  })));
  storage_directory->onWillRemoveItem.connect(SIG2_TRACK(sig2_t<void (sptr<StorageItem> item, int index)>::slot_type(
    [this](sptr<StorageItem> item, int index) {
    removeStorageItemView(item);
  })));
}

void DirectoryView::addStorageItemView(sptr<StorageItem> storage_item, int index) {
  auto factory = StorageItemViewFactoryRegistry::getFactory(storage_item->type());
  if (!factory) {
    spdlog::get(LOGGER_DEFAULT)->warn("[DirectoryView] StorageItemViewFactory not found! expected factory type = {}", storage_item->type());
    return;
  }
  auto view = factory->create(nullptr, storage_item);
  if (!view) {
    spdlog::get(LOGGER_DEFAULT)->warn("[DirectoryView] View is null! item type = {}", storage_item->type());
    return;
  }
  auto pair = make_pair(storage_item, view);
  view_items_.insert(view_items_.begin() + index, pair);
  grid_layout_->addWidget(view);
}

void DirectoryView::removeStorageItemView(sptr<StorageItem> storage_item) {
  int i = 0;
  for (auto it : view_items_) {
    if (it.first == storage_item) break;
    i++;
  }
  if (i == view_items_.size()) {
    spdlog::get(LOGGER_DEFAULT)->warn("[DirectoryView] removeStorageItemView failed! View not found.");
    return;
  }
  auto view = view_items_[i].second;
  view_items_.erase(view_items_.begin() + i);
  grid_layout_->removeWidget(view);
  delete view;
}

StorageItemView* DirectoryView::getStorageItemView(sptr<StorageItem> storage_item) {
  for (auto it : view_items_)
    if (it.first == storage_item) return it.second;
  spdlog::get(LOGGER_DEFAULT)->warn("[DirectoryView] getStorageItemView is null! item type = {}", storage_item->type());
  return nullptr;
}

void DirectoryView::contentRectUpdated() {
  grid_layout_->setGeometry(rect());
}

void DirectoryView::paintEvent(QPaintEvent* event) {
}

void DirectoryView::dragEnterEvent(QDragEnterEvent* event) {
  const QMimeData* mime_data = event->mimeData();
  QList<QFileInfo> url_list;

  if (mime_data->hasUrls()) {

    for (auto url : mime_data->urls()) {
      url_list.push_back(QFileInfo(url.toLocalFile()));
      qDebug() << (*url_list.rbegin()).filePath() << " " << (*url_list.rbegin()).suffix() << "\n";
    }

    import_service_->import(url_list, storage_directory_);
  }
}

sptr<StorageDirectory> DirectoryView::storage_directory() {
  return storage_directory_;
}

}

}