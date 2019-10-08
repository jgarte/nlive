#ifndef NLIVE_EFFECT_CONTROL_PROPERTY_VIEW_H_
#define NLIVE_EFFECT_CONTROL_PROPERTY_VIEW_H_

#include <QWidget>
#include <QEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QSharedPointer>
#include "base/common/sig.h"
#include "model/effect/property.h"
#include "browser/widgets/effect_control/property/number_input_box.h"
#include "platform/theme/themeservice.h"

namespace nlive {

class Sequence;
class Clip;

namespace effect {

namespace value {

class Vector2;

}

}

namespace effect_control {

class EffectControlLayout;
class NumberInputBox;

template <class T>
class PropertyAnimateToggleButton : public QPushButton {

private:
  QSharedPointer<IThemeService> theme_service_;

  bool animated_;
  QSharedPointer<T> property_;

  void handleClick() {
    property_->setAnimatable(!property_->animatable());
  }

  void handleDidChangeAnimatable(bool value) {
    animated_ = value;
    update();
  }

protected:
  void paintEvent(QPaintEvent* event) override {
    auto& theme = theme_service_->getTheme();
    QPainter p(this);
    p.setRenderHint(QPainter::RenderHint::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect(), 3, 3);
    if (animated_) {
      p.fillPath(path, theme.surfaceBrightColor());
    }
    else {
      p.strokePath(path, theme.surfaceBrightColor());
    }
  }

public:
  PropertyAnimateToggleButton(
    QWidget* parent,
    QSharedPointer<EffectControlLayout> layout_params,
    QSharedPointer<Sequence> sequence,
    QSharedPointer<Clip> clip,
    QSharedPointer<T> property,
    QSharedPointer<IThemeService> theme_service) :
  theme_service_(theme_service),
  QPushButton(parent) {
    animated_ = property->animated();
  }

};

class PropertyLabelView : public QWidget {

private:
  QString label_;
  int flags_;

protected:
  void paintEvent(QPaintEvent* event) override;

public:
  PropertyLabelView(
    QWidget* parent, QString label, int flags = 0);

  int flags() const;

};

template <class T>
class PropertyView : public QWidget, protected Sig {

private:
  QString label_;
  PropertyAnimateToggleButton<T>* animate_toggle_button_;
  PropertyLabelView* label_view_;
  std::vector<QWidget*> input_views_;

  void handleDidChangeAnimatable(bool value) {
    if (value) {
      if (animate_toggle_button_ != nullptr) return;
      animate_toggle_button_ = new PropertyAnimateToggleButton<T>(
        this, layout_params_, sequence_, clip_, property_, theme_service_);
      animate_toggle_button_->show();
      doLayout();
    }
    else {
      if (animate_toggle_button_ == nullptr) return;
      delete animate_toggle_button_;
      animate_toggle_button_ = nullptr;
    }

  }

  void doLayout() {
    animate_toggle_button_->move(0, 0);
    animate_toggle_button_->resize(20, 20);
    label_view_->move(20, 0);
    label_view_->resize(60, 20);

    int input_view_x = 100;
    for (int i = 0; i < input_views_.size(); i ++) {
      auto input_view = input_views_[i];
      input_view->move(input_view_x, 0);
      input_view->resize(60, 20);
      input_view_x += 80;
    }
  }

protected:
  QSharedPointer<IThemeService> theme_service_;
  QSharedPointer<EffectControlLayout> layout_params_;
  QSharedPointer<Sequence> sequence_;
  QSharedPointer<Clip> clip_;
  QSharedPointer<T> property_;

  virtual void updateValue() = 0;

  bool event(QEvent* event) override {
    switch (event->type()) {
    case QEvent::LayoutRequest:
    case QEvent::Resize:
      doLayout();
      return true;
    }
    return false;
  }

public:
  PropertyView(
    QWidget* parent,
    QSharedPointer<EffectControlLayout> layout_params,
    QSharedPointer<Sequence> sequence,
    QSharedPointer<Clip> clip,
    QSharedPointer<T> property,
    QString label,
    QSharedPointer<IThemeService> theme_service) :
    theme_service_(theme_service),
    QWidget(parent), layout_params_(layout_params),
    sequence_(sequence), clip_(clip), property_(property),
    label_(label), animate_toggle_button_(nullptr) {
    label_view_ = new PropertyLabelView(this, label, Qt::AlignVCenter);
    handleDidChangeAnimatable(property->animatable());
    property->onDidChangeAnimatable.connect(
      sig2_t<void (bool)>::slot_type
      (&PropertyView<T>::handleDidChangeAnimatable, this, _1).track(__sig_scope_));
  }

  void addInputView(QWidget* widget) {
    input_views_.push_back(widget);
  }

  QSize sizeHint() const override {
    return QSize(width(), 20);
  }

};

}

}

#endif