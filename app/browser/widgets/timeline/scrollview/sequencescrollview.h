#ifndef NLIVE_SEQUENCE_SCROLL_VIEW_H_
#define NLIVE_SEQUENCE_SCROLL_VIEW_H_

#include <QWidget>
#include "base/layout/div.h"
#include "base/common/sig.h"
#include "browser/widgets/timeline/scrollview/scrollbar.h"

namespace nlive {

class Sequence;
class IThemeService;

class SequenceScrollView : public Div {
  Q_OBJECT

private:
  class Indicator : public QWidget {
    private:
      sptr<IThemeService> theme_service_;
    protected:
      void paintEvent(QPaintEvent* event) override;
    public:
      Indicator(QWidget* parent, sptr<IThemeService> theme_service);
  };

  sptr<IThemeService> theme_service_;

  sptr<Sequence> sequence_;

  QWidget* content_widget_;
  SequenceScrollViewScrollbar scrollbar_;

  Indicator indicator_;

  int min_start_time_;
  int max_end_time_;
  int start_time_;
  int end_time_;
  qreal px_per_frame_;
  qreal unit_frame_time_;
  qreal unit_width_;

  void doUpdate();
  void doResize();

protected:
  void mouseMoveEvent(QMouseEvent* event) override;

  void contentRectUpdated() override;
  void paintEvent(QPaintEvent* event) override;

public:
  SequenceScrollView(
    QWidget* const parent,
    QWidget* content_widget,
    sptr<Sequence> sequence,
    sptr<IThemeService> theme_service);

  void setContentWidget(QWidget* widget);

  void update(qreal scroll_start, qreal scroll_end);

  int getTimeRelativeToView(int pixel) const;
  int getTimeAmountRelativeToView(int pixel) const;
  int getPositionRelativeToView(int time) const;
  int getPixelAmountRelativeToView(int time) const;

  void setMinStartTime(int value);
  void setMaxEndTime(int value);

  inline QWidget* content_widget() const { return content_widget_; }
  inline int start_time() const { return start_time_; }
  inline int end_time() const { return end_time_; }
  inline qreal px_per_frame() const { return px_per_frame_; }
  inline qreal unit_frame_time() const { return unit_frame_time_; }
  inline qreal unit_width() const { return unit_width_; }

  sig2_t<void ()> onDidUpdate;

};

}

#endif