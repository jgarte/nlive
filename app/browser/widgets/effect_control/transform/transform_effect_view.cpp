#include "browser/widgets/effect_control/transform/transform_effect_view.h"

#include <QDebug>

#include "platform/logger/logger.h"
#include "model/sequence/sequence.h"
#include "model/sequence/clip.h"
#include "model/effect/transform_effect.h"
#include "browser/widgets/effect_control/property/vector2_property_view.h"

namespace nlive {

namespace effect_control {

TransformEffectView::TransformEffectView(
  QWidget* parent,
  QSharedPointer<EffectControlLayout> layout_params,
  QSharedPointer<Sequence> sequence,
  QSharedPointer<Clip> clip,
  QSharedPointer<effect::TransformEffect> effect,
  QSharedPointer<IThemeService> theme_service) :
  EffectView(parent, layout_params, sequence, clip, effect, theme_service) {
  position_property_view_ =
      new Vector2PropertyView(this, layout_params, sequence, clip, effect->position(), QString("Position"), theme_service);
  insertPropertyView(position_property_view_, 0);
}

TransformEffectViewFactory::TransformEffectViewFactory() {}
TransformEffectView* TransformEffectViewFactory::create(
    QWidget* parent,
    QSharedPointer<EffectControlLayout> layout_params,
    QSharedPointer<Sequence> sequence,
    QSharedPointer<Clip> clip,
    QSharedPointer<effect::Effect> effect,
    QSharedPointer<IThemeService> theme_service) {
  if (effect->type() != effect::TransformEffect::TYPE) {
    spdlog::get(LOGGER_DEFAULT)->warn(
      "[TransformEffectViewFactory] Effect type not match! expected = {}, got = {}", effect::TransformEffect::TYPE, effect->type());
    return nullptr;
  }
  QSharedPointer<effect::TransformEffect> transform_effect = effect.staticCast<effect::TransformEffect>();
  return new TransformEffectView(parent, layout_params, sequence,
    clip, transform_effect, theme_service);
}


}

}