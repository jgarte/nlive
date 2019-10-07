#ifndef NLIVE_EFFECT_CONTROL_CLIP_VIEW_H_
#define NLIVE_EFFECT_CONTROL_CLIP_VIEW_H_

#include <QWidget>
#include <QVBoxLayout>
#include <QSharedPointer>
#include <vector>
#include <map>

namespace nlive {

class IThemeService;
class Sequence;
class Clip;

namespace effect {

class Effect;

}

namespace effect_control {

class EffectControlLayout;
class EffectView;

class ClipSideView {

private:
  QSharedPointer<Sequence> sequence_;
  QSharedPointer<Clip> clip_;
  
public:
  ClipSideView(
    QSharedPointer<Clip> clip);

};

class ClipTimelineView {

private:
  QSharedPointer<Sequence> sequence_;
  QSharedPointer<Clip> clip_;
  
public:
  ClipTimelineView(
    QSharedPointer<Clip> clip);

};

class ClipView : public QWidget {
  Q_OBJECT

private:
  QSharedPointer<IThemeService> theme_service_;
  QSharedPointer<EffectControlLayout> layout_params_;
  QSharedPointer<Sequence> sequence_;
  QSharedPointer<Clip> clip_;
  ClipSideView* side_view_;
  ClipTimelineView* timeline_view_;

  int height_;

  std::vector<std::pair<QSharedPointer<effect::Effect>, EffectView*>> effect_views_;
  std::map<QSharedPointer<effect::Effect>, EffectView*> effect_view_map_;

  void addEffectView(QSharedPointer<effect::Effect> effect, int index);

  void doLayout();

protected:
  bool event(QEvent* event) override;

public:
  ClipView(
    QWidget* parent,
    QSharedPointer<EffectControlLayout> layout,
    QSharedPointer<Sequence> sequence,
    QSharedPointer<Clip> clip,
    QSharedPointer<IThemeService> theme_service);

  QSize sizeHint() const;

};

}

}

#endif