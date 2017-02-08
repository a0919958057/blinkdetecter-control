#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QFile>
#include <QMessageBox>


#include "imageloader.h"
#include "detectorplot.h"
#include "samplingthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
    void on_action_Online_Mode_triggered(bool checked);

    void on_action_Video_Mode_triggered(bool checked);

    void on_actionLoad_Video_triggered();

    void on_actionExit_triggered();

    void on_btn_start_clicked();

    void on_btn_stop_clicked();

    void on_checkBox_blink_clicked(bool checked);

    void on_pushButton_clicked();

    void on_btn_clearplot_clicked();

    void on_checkBox_clicked(bool checked);

public slots:
    void updateImageshow(const QImage& image);
    void updateModifyImageshow(const QImage& image);
    void updateEyeImageshow(const QImage &image);

private:
  Ui::MainWindow *ui;

  QActionGroup *m_mode_group;
  ImageLoader *m_image_loader;
  int d_timerId;
  DetectorPlot* plotter;
  SamplingThread* plotterSamplingThread;

  // QObject interface
protected:
  virtual void timerEvent(QTimerEvent *event);
};

#endif // MAINWINDOW_H
