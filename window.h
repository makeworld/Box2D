#ifndef WINDOW_H
#define WINDOW_H

#include "render.h"
#include "b2thread.h"
#include <QWindow>
#include <QScopedPointer>
#include <QExposeEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QSurfaceFormat>

class MainWindow : public QWindow
{
public:
  explicit MainWindow(QWindow* parent = 0);
  virtual ~MainWindow();

protected:
  void exposeEvent(QExposeEvent*);
  void mousePressEvent(QMouseEvent*);
  void resizeEvent(QResizeEvent*);

private:
  QScopedPointer<Renderer> m_renderer;
  QScopedPointer<B2Thread> m_b2thread;

private slots:
};

#endif // WINDOW_H
