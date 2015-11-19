#ifndef B2THREAD_H
#define B2THREAD_H

#include "Box2D/Box2D.h"
#include <QScopedPointer>
#include <QtMath>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QElapsedTimer>

class B2Thread : public QThread, public b2Draw
{
  Q_OBJECT

public:
  B2Thread(QObject*);
  virtual ~B2Thread();

  void quit(){ m_playing = false; }
  void add(float x, float y);

  void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
  {
    emit drawPolygon(vertices, vertexCount, color);
  }
  void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
  {
    emit drawSolidPolygon(vertices, vertexCount, color);
  }
  void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
  {
    emit drawCircle(center, radius, color);
  }
  void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
  {
    emit drawSolidCircle(center, radius, axis, color);
  }
  void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
  {
    emit drawSegment(p1, p2, color);
  }
  void DrawTransform(const b2Transform& xf)
  {
    emit drawTransform(xf);
  }
  void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
  {
    emit drawPoint(p, size, color);
  }
  void DrawString(int, int, const char*, ...)
  {
  }
  void DrawString(const b2Vec2&, const char*, ...)
  {
  }
  void DrawAABB(b2AABB* aabb, const b2Color& color)
  {
    emit drawAABB(aabb, color);
  }

private:
  void init();
  void run();

  QMutex m_mutex;
  b2World m_b2world;

  volatile bool m_playing;

signals:
  void preDraw();
  void postDraw();
  void drawPolygon(const b2Vec2*, int32, const b2Color&);
  void drawSolidPolygon(const b2Vec2*, int32, const b2Color&);
  void drawCircle(const b2Vec2&, float32, const b2Color&);
  void drawSolidCircle(const b2Vec2&, float32, const b2Vec2&, const b2Color&);
  void drawSegment(const b2Vec2&, const b2Vec2&, const b2Color&);
  void drawTransform(const b2Transform&);
  void drawPoint(const b2Vec2&, float32, const b2Color&);
  void drawAABB(b2AABB*, const b2Color&);
};

#endif // B2THREAD_H
