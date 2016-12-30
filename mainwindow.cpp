#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  m_mode_group = new QActionGroup(this);
  m_mode_group->addAction(ui->action_Online_Mode);
  m_mode_group->addAction(ui->action_Video_Mode);

  m_image_loader = new ImageLoader(this);

  connect(m_image_loader, SIGNAL(processedImage(QImage)), this, SLOT(updateImageshow(QImage)));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_action_Online_Mode_triggered(bool checked)
{
    if(checked) {
        emit on_btn_stop_clicked();
        m_image_loader->setMode(ImageLoader::ONLINE_MODE);
        int camera_count = m_image_loader->getCameraCount();
        QStringList cameraList;
        bool ok;
        for(int i=0; i< camera_count; i++) {
            cameraList << QString::number(i);
        }

        QString camera_selected = QInputDialog::getItem(this, tr("Select camera dialog"),tr("Please select camera："),cameraList,0,false,&ok);
        if (ok && !camera_selected.isEmpty()){
            m_image_loader->loadCamera(camera_selected.toInt());
        }

    }
}

void MainWindow::on_action_Video_Mode_triggered(bool checked)
{
    if(checked) {
        emit on_btn_stop_clicked();
        m_image_loader->setMode(ImageLoader::VIDEO_MODE);
        if(m_image_loader->isFileSeted())
            m_image_loader->loadVideo(m_image_loader->getSetedFileName());
        else
            emit on_actionLoad_Video_triggered();
    }
}

void MainWindow::on_actionLoad_Video_triggered()
{
    ui->action_Video_Mode->setChecked(true);
    m_image_loader->loadVideo(QFileDialog::getOpenFileName(
                                  this,
                                  tr("Open Video File"),
                                  QDir::homePath(),
                                  tr("Video Files (*.avi *.mpg *.mp4 *.webm)")));

}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::updateImageshow(const QImage& image)
{
    ui->imageshow_raw->setPixmap(QPixmap::fromImage(image, Qt::AutoColor).scaled(ui->imageshow_raw->size(),Qt::KeepAspectRatio));
}

void MainWindow::on_btn_start_clicked()
{
    if(!m_image_loader->isReady()) {
        QMessageBox::critical(this,tr("Image Loader Error"),tr("Image source not ready yet!"));
        return ;
    }
    m_image_loader->Start();
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(true);

}

void MainWindow::on_btn_stop_clicked()
{
    m_image_loader->Stop();
    ui->btn_start->setEnabled(true);
    ui->btn_stop->setEnabled(false);
}
