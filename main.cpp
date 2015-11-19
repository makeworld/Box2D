#include "window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  MainWindow window;
  window.resize(480, 480);
  window.show();

  return a.exec();
}
