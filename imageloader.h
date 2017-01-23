#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QThread>
#include <QMutex>
#include <QImage>
#include <QWaitCondition>

#include <opencv2/opencv.hpp>

#include "EyeDetector.h"

using namespace cv;

class ImageLoader : public QThread
{
public:
    enum CaptureMode {
        ONLINE_MODE,
        VIDEO_MODE,
        SAVEING_MODE
    };

    enum ErrorEvent {
        ERROR_MODE_INCORRECT
    };

    Q_OBJECT
public:
    explicit ImageLoader(QObject *parent = 0);
    ~ImageLoader();

signals:
    void processedImage(const QImage& image);
    void processedModifyImage(const QImage& image);
    void processedEyeImage(const QImage& image);
    void errorEvent(const ErrorEvent& event);

protected:
    void run();
    void msleep(int ms);

public:

    // Load the video from file
    bool loadVideo(QString filename);

    // Load the video from file
    bool loadCamera(int camera_id);

    // Set the Capture mode
    void setMode(const CaptureMode& amode);

    // Start the capture
    void Start();

    // Stop the capture
    void Stop();

    //check if the player has been stopped
    bool isStopped() const;

    // check if there is ready
    bool isReady() const;

    // check if there is already file import
    bool isFileSeted() const;

    // getSetedFileName
    QString getSetedFileName();

    // get usable camera count
    int getCameraCount();

public slots:


public:
    EyeDetector m_eyedetector;

private:
    bool isStop;
    CaptureMode mode;
    QMutex mutex;
    QWaitCondition condition;

    int frame_rate;
    VideoCapture capture;

    QString main_filename;

    CascadeClassifier faceCascade;
    CascadeClassifier eyeCascade;

    std::vector<Rect> face_vec;
    std::vector<Rect> eye_vec;

    void initDetector();
    QImage cvt2QImage(Mat frame);
};

#endif // IMAGELOADER_H
