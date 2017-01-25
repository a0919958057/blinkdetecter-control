#include "mainwindow.h"
#include <QApplication>
#include "detectorplot.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.showMaximized();

  //DetectorPlot plot;
  //plot.show();

  return a.exec();
}
