#ifndef _NLIVE_PROJECT_WIDGET_DIRECTORY_VIEW_H_
#define _NLIVE_PROJECT_WIDGET_DIRECTORY_VIEW_H_

#include <QWidget>
#include <QSharedPointer>
#include <vector>
#include <map>

#include "base/layout/div.h"
#include "base/layout/grid_layout.h"

namespace nlive {

class IThemeService;
class IImportService;
class StorageItem;
class StorageDirectory;

namespace project_widget {
  
class StorageItemView;

class DirectoryView : public Div {

private:
  QSharedPointer<IThemeService> theme_service_;
  QSharedPointer<IImportService> import_service_;
  QSharedPointer<StorageDirectory> storage_directory_;

  std::vector<std::pair<QSharedPointer<StorageItem>, StorageItemView*>> view_items_;
  
  GridLayout* grid_layout_;

  void addStorageItemView(QSharedPointer<StorageItem> storage_item, int index);
  void removeStorageItemView(QSharedPointer<StorageItem> storage_item);
  StorageItemView* getStorageItemView(QSharedPointer<StorageItem> storage_item);

protected:
  void contentRectUpdated() override;
  void paintEvent(QPaintEvent* event) override;
  void dragEnterEvent(QDragEnterEvent* event) override;

public:
  DirectoryView(
    QWidget* parent,
    QSharedPointer<StorageDirectory> storage_directory,
    QSharedPointer<IThemeService> theme_service,
    QSharedPointer<IImportService> import_service);

  QSharedPointer<StorageDirectory> storage_directory();
  
};

}

}

#endif