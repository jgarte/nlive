#ifndef NLIVE_EFFECT_CONTROL_TRANSFORM_EFFECT_VIEW_H_
#define NLIVE_EFFECT_CONTROL_TRANSFORM_EFFECT_VIEW_H_

#include <QWidget>
#include <QSharedPointer>
#include <vector>

#include "browser/widgets/effect_control/effect_view.h"
#include "browser/widgets/effect_control/effect_view_factory.h"

namespace nlive {

class Sequence;
class Clip;

namespace effect {

class Effect;
class TransformEffect;

namespace value {

class Vector2;

}

}

namespace effect_control {

class EffectControlLayout;
class Vector2PropertyView;

class TransformEffectView : public EffectView {
  Q_OBJECT

private:
  Vector2PropertyView* position_property_view_;

public:
  TransformEffectView(
    QWidget* parent,
    QSharedPointer<EffectControlLayout> layout,
    QSharedPointer<Sequence> sequence,
    QSharedPointer<Clip> clip,
    QSharedPointer<effect::TransformEffect> effect,
    QSharedPointer<IThemeService> theme_service);

};

class TransformEffectViewFactory : public EffectViewFactory {

public:
  TransformEffectViewFactory();
  TransformEffectView* create(
    QWidget* parent,
    QSharedPointer<EffectControlLayout> layout_params,
    QSharedPointer<Sequence> sequence,
    QSharedPointer<Clip> clip,
    QSharedPointer<effect::Effect> effect,
    QSharedPointer<IThemeService> theme_service) override;

};

}

}

#endif