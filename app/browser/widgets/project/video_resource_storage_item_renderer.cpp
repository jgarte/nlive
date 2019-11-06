#include "browser/widgets/project/video_resource_storage_item_renderer.h"

#include <QPainter>
#include "model/resource/video_resource.h"

namespace nlive {

namespace project_widget {

VideoResourceStorageItemThumbnailView::VideoResourceStorageItemThumbnailView(
  QWidget* parent, QSharedPointer<VideoResourceStorageItem> item) :
  Div(parent) {
  QSharedPointer<VideoResourceStorageItem> vrsi = item.staticCast<VideoResourceStorageItem>();
  QSharedPointer<VideoResource> vs = vrsi->video_resource();
  VideoDecoder dec(item->resource()->path());
  auto frame = dec.decode(0, true);
  uint8_t* rgb = new uint8_t[vs->width() * vs->height() * 3];
  frame->scale(rgb, AV_PIX_FMT_RGB24);
  image_ = new QImage(rgb, vs->width(), vs->height(),
    vs->width() * 3, QImage::Format_RGB888);
}

VideoResourceStorageItemThumbnailView::~VideoResourceStorageItemThumbnailView() {
  delete image_;
}

void VideoResourceStorageItemThumbnailView::paintEvent(QPaintEvent* e) {
  QPainter p(this);
  p.drawImage(rect(), *image_);
}

VideoResourceStorageItemRenderer::VideoResourceStorageItemRenderer(
    QSharedPointer<VideoResourceStorageItem> item,
    QOpenGLContext* target_gl) : item_(item), target_gl_(target_gl) {
  
}

void VideoResourceStorageItemRenderer::open() {
  auto resource = item_->video_resource();
  renderer_ = QSharedPointer<video_renderer::Renderer>(
    new video_renderer::Renderer(target_gl_, resource->width(), resource->height()));
}

void VideoResourceStorageItemRenderer::close() {
  renderer_ = nullptr;
}

}

}