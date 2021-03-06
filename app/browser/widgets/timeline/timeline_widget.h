#ifndef NLIVE_TIMELINE_WIDGET_H_
#define NLIVE_TIMELINE_WIDGET_H_

#include <QDockWidget>
#include "base/common/memory.h"
#include <QLayout>

#include "base/layout/fillparentbox.h"
#include "base/common/sig.h"
#include "platform/include.h"
#include "model/include.h"
#include "browser/widgets/widget.h"
#include "browser/services/include.h"
#include "browser/services/play/play_service.h"
#include "browser/services/projects/projects_service.h"

namespace nlive {

class ITimelineWidgetService;

namespace timeline_widget {

class SequenceView;

class TimelineWidget : public Widget {
  Q_OBJECT;

  DECLARE_WIDGET("nlive.widget.Timeline");

private:
  sptr<IThemeService> theme_service_;
  sptr<PlayService> play_service_;
  sptr<ITimelineWidgetService> timeline_widget_service_;
  sptr<IProjectsService> projects_service_;
  sptr<SequencesService> sequences_service_;

  sptr<SequenceStorageItem> sequence_storage_item_;
  SequenceView* sequence_view_;

protected:
  void resizeEvent(QResizeEvent* event) override;
  void onFocused() override;
  void onBlured() override;

public:
  TimelineWidget(
    QWidget* parent,
    sptr<IThemeService> themeService,
    sptr<ITimelineWidgetService> timeline_widget_service,
    sptr<PlayService> play_service,
    sptr<IProjectsService> projects_service,
    sptr<SequencesService> sequences_service,
    sptr<IWidgetsService> widgets_service);

  void setSequenceStorageItem(sptr<SequenceStorageItem> sequence_storage_item);

  sptr<SequenceStorageItem> sequence_storage_item();
  SequenceView* sequence_view();
  
  sig2_t<void (sptr<SequenceStorageItem>)> onDidChangeSequenceStorageItem;

};

}

}

#endif