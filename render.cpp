#include "render.h"
#include "window.h"

Renderer::Renderer(const QSurfaceFormat& format)
  : m_format(format)
  , m_initialized(false)
  , m_window(nullptr)
{
  m_context.reset(new QOpenGLContext(this));
  m_context->setFormat(format);
  m_context->create();
}

Renderer::~Renderer()
{
}

void Renderer::initGL()
{
  m_program.reset(new QOpenGLShaderProgram(this));
  m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl");
  m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl");
  m_program->link();
  m_program->bind();

  m_vertexLocation = m_program->attributeLocation("vertex");
  m_program->enableAttributeArray(m_vertexLocation);

  m_matrixLocation = m_program->uniformLocation("matrix");
  m_colorLocation = m_program->uniformLocation("color");

  QOpenGLFunctions* f = m_context->functions();
  f->glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

  m_projectionMatrix.setToIdentity();
  m_projectionMatrix.ortho(0.0f, 10.0f, 0.0f, 10.0f, -1.0f, 1.0f);
}

void Renderer::start(MainWindow* window)
{
  m_window = window;
  QTimer::singleShot(0, this, SLOT(render()));
}

void Renderer::resize()
{
  {
    QMutexLocker locker(&m_mutex);
    m_commandQueue.append(DrawCommand(DrawCommandType::Resize));
  }

  QTimer::singleShot(0, this, SLOT(render()));
}

void Renderer::preDraw()
{
  {
    QMutexLocker locker(&m_mutex);
    m_commandQueue.append(DrawCommand(DrawCommandType::PreDraw));
  }

  QTimer::singleShot(0, this, SLOT(render()));
}

void Renderer::postDraw()
{
  {
    QMutexLocker locker(&m_mutex);
    m_commandQueue.append(DrawCommand(DrawCommandType::PostDraw));
  }

  QTimer::singleShot(0, this, SLOT(render()));
}

void Renderer::drawSolidPolygon(const b2Vec2* vertices, int32 verticesCount, const b2Color& color)
{
  DrawCommand cmd(DrawCommandType::Polygon);

  QVector<QVector2D> vec(verticesCount);
  for(int32 i = 0; i < verticesCount; ++i){
    vec[i].setX(vertices[i].x);
    vec[i].setY(vertices[i].y);
  }
  cmd.setVertices(vec);

  cmd.setColor(QColor::fromRgbF(color.r, color.g, color.b));

  {
    QMutexLocker locker(&m_mutex);
    m_commandQueue.append(cmd);
  }

  QTimer::singleShot(0, this, SLOT(render()));
}

void Renderer::render()
{
  if(!m_context->makeCurrent(m_window)){
    return;
  }

  if(!m_initialized){
    initGL();
    m_initialized = true;
  }

  QOpenGLFunctions* f = m_context->functions();

  DrawCommand cmd;
  {
    QMutexLocker locker(&m_mutex);
    if(!m_commandQueue.isEmpty()){
      cmd = m_commandQueue.dequeue();
    }
  }

  switch(cmd.type()){
  case DrawCommandType::Resize:
    f->glViewport(0, 0, m_window->width(), m_window->height());
    break;

  case DrawCommandType::PreDraw:
    f->glClear(GL_COLOR_BUFFER_BIT);
    m_program->bind();
    break;

  case DrawCommandType::PostDraw:
    m_program->release();
    m_context->swapBuffers(m_window);
    break;

  case DrawCommandType::Polygon:
    m_program->setUniformValue(m_matrixLocation, m_projectionMatrix);
    m_program->setUniformValue(m_colorLocation, cmd.color());
    m_program->setAttributeArray(m_vertexLocation, cmd.vertices().constData());
    f->glDrawArrays(GL_TRIANGLE_FAN, 0, cmd.vertices().count());
    break;

  default:
    break;
  }
}
