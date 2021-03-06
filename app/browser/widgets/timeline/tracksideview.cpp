#include "browser/widgets/timeline/tracksideview.h"

#include "platform/theme/themeservice.h"
#include "model/sequence/track.h"

#include <QPainter>
#include <QDebug>

namespace nlive {

namespace timeline_widget {

TrackSideView::TrackSideView(QWidget* parent, sptr<Track> track, sptr<IThemeService> theme_service) :
  QWidget(parent), track_(track), theme_service_(theme_service) {

}

void TrackSideView::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
}

void TrackSideView::paintEvent(QPaintEvent* event) {
  auto& theme = theme_service_->getTheme();
  QPainter p(this);
  QFont f;
  f.setPointSize(9);
  p.setFont(f);
  p.setPen(theme.primaryColor());
  QRect r = rect();
  // padding
  r.setLeft(r.left() + 5);
  p.drawText(r, Qt::AlignVCenter, "Track");
}

}

}