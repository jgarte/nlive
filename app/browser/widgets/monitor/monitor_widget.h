#ifndef NLIVE_MONITOR_WIDGET_H_
#define NLIVE_MONITOR_WIDGET_H_

#include <QDockWidget>
#include "base/common/memory.h"
#include <vector>
#include "browser/widgets/widget.h"
#include "base/common/sig.h"
#include "base/layout/include.h"
#include "browser/services/play/play_service.h"
#include "browser/widgets/timeline/timeline_widget.h"
#include "browser/widgets/timeline/sequenceview.h"
#include "browser/widgets/monitor/actions.h"
#include "base/layout/action_bar.h"

namespace nlive {

class ITimelineWidgetService;
class IThemeService;
class Sequence;

namespace monitor_widget {

class SequenceView;

class MonitorWidget : public Widget {
  DECLARE_WIDGET("nlive.widget.Monitor");

private:
  sptr<ITimelineWidgetService> timeline_widget_service_;
  sptr<IThemeService> theme_service_;
  sptr<PlayService> play_service_;

  std::vector<sig2_conn_t> widget_connections_;
  SequenceView* sequence_view_;
  
  sptr<ActionContext> action_context_;
  ActionBar* action_bar_;

  FlexLayout* container_;
  FillParentBox* sequence_view_container_;
  FillParentBox* action_bar_container_;

  void handleDidChangeCurrentTimelineWidget(timeline_widget::TimelineWidget* timeline_widget);
  void handleDidChangeSequenceView(timeline_widget::SequenceView* sequence_view);

protected:
  void resizeEvent(QResizeEvent* event) override;

public:
  MonitorWidget(QWidget* parent,
    sptr<ITimelineWidgetService> timeline_widget_service,
    sptr<IThemeService> theme_service,
    sptr<PlayService> play_service,
    sptr<IWidgetsService> widgets_service);

};

}

}

#endif