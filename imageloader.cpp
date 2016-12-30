#include "imageloader.h"

ImageLoader::ImageLoader(QObject *parent) : QThread(parent)
{
    isStop = true;
    mode = ONLINE_MODE;
    //capture.open(0);
}

ImageLoader::~ImageLoader() {
    mutex.lock();
    isStop = true;
    capture.release();
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void ImageLoader::run()
{
    int delay = (1000/frame_rate);

    if(capture.isOpened()) {
        while(!isStop) {
            if (!capture.read(frame))
            {
                isStop = true;
            }
            if(frame.channels() == 3) {
                cvtColor(frame, frame_RGB, CV_BGR2RGB);
                img = QImage(
                            reinterpret_cast<const unsigned char*>(frame_RGB.data),
                            frame_RGB.cols,
                            frame_RGB.rows,
                            QImage::Format_RGB888);
            }
            else {
                img = QImage(
                            reinterpret_cast<const unsigned char*>(frame_RGB.data),
                            frame_RGB.cols,
                            frame_RGB.rows,
                            QImage::Format_Grayscale8);
            }
            emit processedImage(img);
            //
            this->msleep(delay);
        }

    }
}

void ImageLoader::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}

bool ImageLoader::loadVideo(QString filename)
{
    main_filename = filename;
    if(mode != VIDEO_MODE) {
        emit errorEvent(ERROR_MODE_INCORRECT);
        return false;
    }

    capture.open(filename.toStdString().c_str());

    if (capture.isOpened())
    {
        frame_rate = static_cast<int>(capture.get(CV_CAP_PROP_FPS));
        if(frame_rate == 1000) frame_rate = 30;

        return true;
    }
    else
        return false;
}

bool ImageLoader::loadCamera(int camera_id)
{
    if(mode != ONLINE_MODE && mode!= SAVEING_MODE) {
        emit errorEvent(ERROR_MODE_INCORRECT);
        return false;
    }

    capture.open(camera_id);

    if(capture.isOpened()) {
        frame_rate = static_cast<int>(capture.get(CV_CAP_PROP_FPS));

        return true;
    }
    else {
        return false;
    }

    return true;
}

void ImageLoader::setMode(const ImageLoader::CaptureMode &amode)
{
    mode = amode;
}

void ImageLoader::Start()
{
    if( !isRunning())  {
        if(isStopped()) {
            isStop = false;
        }
        start(LowPriority);
    }
}

void ImageLoader::Stop()
{
    if(mode == VIDEO_MODE) {
        capture.set(CV_CAP_PROP_POS_FRAMES, 0);
    }
    isStop = true;
}

bool ImageLoader::isStopped() const
{
    return isStop;
}

bool ImageLoader::isReady() const
{
    return capture.isOpened();
}

bool ImageLoader::isFileSeted() const
{
    return !main_filename.isEmpty();
}

QString ImageLoader::getSetedFileName()
{
    return main_filename;
}

int ImageLoader::getCameraCount()
{
    return 5;
    cv::VideoCapture camera;
    int device_counts = 0;
    while ( true ) {

        if ( !camera.open(device_counts++) ) {
            break;
        }

    }
    camera.release();
    return (device_counts - 1);
}
