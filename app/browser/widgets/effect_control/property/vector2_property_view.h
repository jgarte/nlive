#ifndef NLIVE_EFFECT_CONTROL_VECTOR2_PROPERTY_VIEW_H_
#define NLIVE_EFFECT_CONTROL_VECTOR2_PROPERTY_VIEW_H_

#include <QWidget>
#include "base/common/memory.h"
#include "model/effect/property.h"
#include "model/effect/value/vector2.h"
#include "browser/widgets/effect_control/property/property_view.h"

namespace nlive {

class Sequence;
class Clip;

namespace effect_control {

class EffectControlLayout;
class NumberInputBox;

class Vector2PropertyView : public PropertyViewTpl<effect::value::Vector2> {

private:
  NumberInputBox* x_input_box_;
  NumberInputBox* y_input_box_;

protected:
  void updateValue() override;

public:
  Vector2PropertyView(
    QWidget* parent,
    sptr<IKeyframesController> keyframes_controller,
    sptr<EffectControlLayout> layout,
    sptr<Sequence> sequence,
    sptr<Clip> clip,
    sptr<effect::Property<effect::value::Vector2>> property,
    QString label,
    SequenceScrollView* sequence_scroll_view,
    sptr<IThemeService> theme_service);

  inline NumberInputBox* x_input_box() { return x_input_box_; }
  inline NumberInputBox* y_input_box() { return y_input_box_; }

};

}

}

#endif