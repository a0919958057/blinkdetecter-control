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

public slots:
    void updateImageshow(const QImage& image);

private:
  Ui::MainWindow *ui;

  QActionGroup *m_mode_group;
  ImageLoader *m_image_loader;
};

#endif // MAINWINDOW_H
