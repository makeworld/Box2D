#include "window.h"

MainWindow::MainWindow(QWindow* parent)
  : QWindow(parent)
{
  QSurfaceFormat format;
  format.setDepthBufferSize(24);

  m_renderer.reset(new Renderer(format));
  m_b2thread.reset(new B2Thread(this));

  setSurfaceType(QWindow::OpenGLSurface);
  setFormat(format);

  connect(m_b2thread.data(), &B2Thread::preDraw,
          m_renderer.data(), &Renderer::preDraw,
          Qt::DirectConnection);

  connect(m_b2thread.data(), &B2Thread::postDraw,
          m_renderer.data(), &Renderer::postDraw,
          Qt::DirectConnection);

  connect(m_b2thread.data(), &B2Thread::drawSolidPolygon,
          m_renderer.data(), &Renderer::drawSolidPolygon,
          Qt::DirectConnection);
}

MainWindow::~MainWindow()
{
}

void MainWindow::exposeEvent(QExposeEvent*)
{
  if(!m_b2thread->isRunning()){
    emit m_renderer->start(this);
    m_b2thread->start();
  }
}

void MainWindow::mousePressEvent(QMouseEvent* ev)
{
  float x = static_cast<float>(ev->x()) / width() * 10.0f;
  float y = (1.0f - static_cast<float>(ev->y()) / height()) * 10.0f;

  m_b2thread->add(x, y);
}

void MainWindow::resizeEvent(QResizeEvent*)
{
  emit m_renderer->resize();
}
