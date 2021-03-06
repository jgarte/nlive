#include "browser/widgets/effect_control/effect_control_widget.h"

#include <QDebug>
#include <QPainter>

#include "model/sequence/sequence.h"
#include "model/effect/transform_effect.h"
#include "browser/widgets/timeline/timeline_widget_service.h"
#include "browser/widgets/timeline/sequenceview.h"
#include "browser/widgets/effect_control/sequence_view.h"
#include "browser/widgets/effect_control/effect_view_factory.h"

#include "browser/widgets/effect_control/transform/transform_effect_view.h"
#include "browser/widgets/effect_control/effects/gray_scale_effect_view.h"
#include "browser/widgets/effect_control/keyframes_controller_impl.h"
#include "browser/widgets/effect_control/actions.h"

namespace nlive {

namespace effect_control {

void EffectControlWidget::Initialize() {
  // Register pre-defined views
  EffectViewFactoryRegistry::registerFactory(
    effect::TransformEffect::TYPE,
    new TransformEffectViewFactory());
  EffectViewFactoryRegistry::registerFactory(
    effect::GrayScaleEffect::TYPE,
    new GrayScaleEffectViewFactory());
  registerEffectControlContributions();
}

EffectControlWidget::EffectControlWidget(
  QWidget* parent,
  sptr<IThemeService> theme_service,
  sptr<ITimelineWidgetService> timeline_widget_service,
  sptr<IMementoService> memento_service,
  sptr<IWidgetsService> widgets_service) :
  Widget(parent, widgets_service, theme_service), theme_service_(theme_service), memento_service_(memento_service),
  timeline_widget_service_(timeline_widget_service),
  target_timeline_widget_(nullptr),
  sequence_view_(nullptr) {
  keyframes_controller_ = sptr<IKeyframesController>(new KeyframesController(this));
  layout_params_ = sptr<EffectControlLayout>(new EffectControlLayout(0.5));
  timeline_widget_service_->onDidChangeCurrentWidget.connect(
    sig2_t<void (timeline_widget::TimelineWidget*)>::slot_type(
    [this](timeline_widget::TimelineWidget* widget) {
      this->setTargetTimelineWidget(widget);
    }).track(__sig_scope_));
}

void EffectControlWidget::setTargetTimelineWidget(timeline_widget::TimelineWidget* timeline_widget) {
  qDebug() << "[EffectControlWidget] setTargetTimelineWidget " << timeline_widget << "\n";
  if (target_timeline_widget_ != nullptr) {
    target_timeline_widget_ = nullptr;
  }
  if (timeline_widget == nullptr) {
    setTargetTimelineWidgetSequenceView(nullptr);
    return;
  }
  target_timeline_widget_ = timeline_widget;
  setTargetTimelineWidgetSequenceView(timeline_widget->sequence_view());
  timeline_widget_connection_.disconnect();
  timeline_widget_connection_ = timeline_widget->onDidChangeSequenceStorageItem.connect(
    sig2_t<void (sptr<SequenceStorageItem>)>::slot_type(
      [this, timeline_widget](sptr<SequenceStorageItem> ssi) {
      this->setTargetTimelineWidgetSequenceView(timeline_widget->sequence_view());
    }));
}

void EffectControlWidget::setTargetTimelineWidgetSequenceView(timeline_widget::SequenceView* sequence_view) {
  qDebug() << "[EffectControlWidget] setTargetTimelineWidget " << sequence_view << "\n";
  if (sequence_view_ != nullptr)
    delete sequence_view_;
  if (sequence_view == nullptr) return;
  qDebug() << "[EffectControlWidget] Create a SequenceView\n";
  sequence_view_ = new SequenceView(this, keyframes_controller_, layout_params_,
    sequence_view, theme_service_, memento_service_);
  sequence_view_->resize(size());
  sequence_view_->show();
  updateGeometry();
}

void EffectControlWidget::resizeEvent(QResizeEvent* event) {
  QDockWidget::resizeEvent(event);
  if (sequence_view_ != nullptr) {
    sequence_view_->setGeometry(0, 0, width(), height() - 15);
  }
}

effect_control::SequenceView* EffectControlWidget::sequence_view() {
  return sequence_view_;
}

sptr<IKeyframesController> EffectControlWidget::keyframes_controller() {
  return keyframes_controller_;
}

}

}