#ifndef RENDER_H
#define RENDER_H

#include "Box2D/Box2D.h"
#include <QScopedPointer>
#include <QQueue>
#include <QVector>
#include <QColor>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

class MainWindow;

namespace DrawCommandType
{
  enum Type { None, Resize, PreDraw, PostDraw, Polygon };
}
class DrawCommand
{
public:
  explicit DrawCommand(DrawCommandType::Type type = DrawCommandType::None) : _type(type)
  {
  }

  DrawCommandType::Type type() const { return _type; }
  const QVector<QVector2D>& vertices() const { return _vertices; }
  const QColor& color() const { return _color; }
  void setVertices(const QVector<QVector2D>& vertices){ _vertices = vertices; }
  void setColor(const QColor& color){ _color = color; }

  const DrawCommand& operator=(const DrawCommand& cmd)
  {
    _type = cmd.type();
    _vertices = cmd.vertices();
    _color = cmd.color();
    return *this;
  }

private:
  DrawCommandType::Type _type;
  QVector<QVector2D> _vertices;
  QColor _color;
};

class Renderer : public QObject
{
  Q_OBJECT

public:
  explicit Renderer(const QSurfaceFormat&);
  virtual ~Renderer();

public slots:
  void start(MainWindow*);
  void resize();
  void preDraw();
  void postDraw();
  void drawSolidPolygon(const b2Vec2*, int32, const b2Color&);

private:
  void initGL();

  QSurfaceFormat m_format;
  QScopedPointer<QOpenGLContext> m_context;
  QScopedPointer<QOpenGLShaderProgram> m_program;
  int m_vertexLocation;
  int m_matrixLocation;
  int m_colorLocation;

  bool m_initialized;
  QMatrix4x4 m_projectionMatrix;

  QMutex m_mutex;
  QQueue<DrawCommand> m_commandQueue;

  MainWindow* m_window;

private slots:
  void render();
};

#endif // RENDER_H
