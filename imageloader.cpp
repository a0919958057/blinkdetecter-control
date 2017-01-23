#include "imageloader.h"

ImageLoader::ImageLoader(QObject *parent) : QThread(parent) {
    faceCascade.load("/home/taiwanet/cascadeFile.xml");
    eyeCascade.load("/home/taiwanet/ojoD.xml");

    // Initizing the EyeDetector with describe XML file;
    m_eyedetector.set_classifier(eyeCascade,eye_vec);

    // Default Stop running with Online mode
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

QImage ImageLoader::cvt2QImage(Mat frame)
{

    Mat frame_RGB;

    if(frame.channels() == 3) {
        cvtColor(frame, frame_RGB, CV_BGR2RGB);
        return QImage(
                    reinterpret_cast<const unsigned char*>(frame_RGB.data),
                    frame.cols,
                    frame.rows,
                    QImage::Format_RGB888);
    }
    else {
        return QImage(
                    reinterpret_cast<const unsigned char*>(frame.data),
                    frame.cols,
                    frame.rows,
                    QImage::Format_Grayscale8);
    }
}

void ImageLoader::run()
{
    int delay = frame_rate;

    if(capture.isOpened()) {

        // Initize the detector with 640x480 OUTPUT and Without Rotation
        initDetector();

        while(!isStop) {

            // TODO: replace capture to m_eyedetector
            if (!m_eyedetector.update_image())
            {
                isStop = true;
            }

            // Process the detect processing
            m_eyedetector.detect_eye();
            Mat eye, modify_frame;
            m_eyedetector.show_frame(modify_frame, eye);


            Mat raw_frame = m_eyedetector.get_raw_frame();
            // TODO: make new SIGNAL to emit Processed Image to GUI
            cv::cvtColor(raw_frame,raw_frame,CV_BGR2RGB);
            QImage raw_image(QImage(
                                 reinterpret_cast<const unsigned char*>(raw_frame.data),
                                 raw_frame.cols,
                                 raw_frame.rows,
                                 QImage::Format_RGB888));

            QImage modify_image(cvt2QImage(modify_frame));

            emit processedImage(raw_image);
            emit processedModifyImage(modify_image);

            if(eye.size != 0) {
                emit processedEyeImage(QImage(
                                           reinterpret_cast<const unsigned char*>(eye.data),
                                           eye.cols,
                                           eye.rows,
                                           QImage::Format_Grayscale8));
            }

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

void ImageLoader::initDetector() {
    m_eyedetector.init(capture, Size(640, 480), RotFlag::ROT_CW_90);
}
