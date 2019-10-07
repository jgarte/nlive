#ifndef _NLIVE_CLIP_H_
#define _NLIVE_CLIP_H_

#include <vector>
#include <QObject>
#include <QDebug>
#include <QSharedPointer>
#include <QUndoStack>

#include "model/common/rational.h"

namespace nlive {

namespace video_renderer {

class CommandBuffer;

}

namespace effect {

class Effect;

}

class Clip : public QObject {
  Q_OBJECT

private:
  int id_;

protected:
  QUndoStack* undo_stack_;

  Rational time_base_;

  int64_t start_time_;
  int64_t end_time_;
  int64_t b_time_;

  std::vector<QSharedPointer<effect::Effect>> effects_;

public:
  Clip(QUndoStack* undo_stack, Rational time_base, int64_t start_time, int64_t end_time, int64_t b_time);
  Clip(const Clip&);

  void setTime(int64_t start_time, int64_t end_time, int64_t b_time);
  void addEffect(QSharedPointer<effect::Effect> effect);

  virtual void render(QSharedPointer<video_renderer::CommandBuffer> command_buffer, int64_t time);

  int64_t start_time() const;
  int64_t end_time() const;
  int64_t b_time() const;

  const std::vector<QSharedPointer<effect::Effect>>& effects();

  int id() const;

  QUndoStack* undo_stack();

  virtual QSharedPointer<Clip> clone() const;

  bool operator<(const Clip& rhs) const;

signals:
  void onDidChangeTime(int64_t old_start_time, int64_t old_end_time, int64_t old_b_time);
  void onDidAddEffect(QSharedPointer<effect::Effect> effect);


};

QDebug operator<<(QDebug dbg, const Clip &type);

struct ClipCompare {
  using is_transparent = void;
  inline bool operator() (const QSharedPointer<Clip>& a, const QSharedPointer<Clip>& b) const
  { return a.get() < b.get(); }
  inline bool operator() (const QSharedPointer<Clip>& a, const Clip* b) const
  { return a.get() < b; }
  inline bool operator() (const Clip* a, const QSharedPointer<Clip>& b) const
  { return a < b.get(); }
};

}

#endif