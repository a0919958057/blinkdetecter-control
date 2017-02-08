#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <QTime>

#include <qmutex.h>

#include "detectdata.h"

using namespace cv;
using namespace std;

#define STATUS_MSG_SIZE (64)
#define WINDOW_NAME_ORIGIN_IMAGE "Origin Image"
#define WINDOW_NAME_MODIFY_IMAGE "Modify Image"
#define WINDOW_NAME_EYE "Eye"

enum EyeDetectorStatus {
	CAMERA_OPENED,
	CAPTURE_EYE,
	CAMERA_CLOSED,
	NO_CAMERA_AVAILABLE
};

enum RotFlag {
	ROT_0,
	ROT_CW_90,
	ROT_CCW_90,
	ROT_180
};

class EyeDetector {
public:
	EyeDetector();
	~EyeDetector();
	void init(VideoCapture&);
	void init(VideoCapture&, Size);
	void init(VideoCapture&, Size, RotFlag);

	bool update_image();

    void show_frame(Mat& modify_frame, Mat& eye_frame);

    bool detect_face(Mat& roi_image);

    bool detect_eye(Mat& roi_image, Size min, Size max);

    void set_classifier(CascadeClassifier &eye_classifier, CascadeClassifier &face_classifier, std::vector<Rect>&, std::vector<Rect>&);

    void set_enable_blink_detect(bool enable);

    void set_enable_face_detect(bool enable);

    Mat get_raw_frame();

    QMutex dataRecord_mutex;
    DetectData dataRecord;

    QMutex pending_mutex;
    double minVal;
    double absSum;

protected:

	void reportStatus(EyeDetectorStatus);

	template<size_t STR_SIZE>
	void reportStatus(EyeDetectorStatus, const char (&)[STR_SIZE]);

	void rot90(Mat&, RotFlag);

	bool detect_blink();

    Mat m_frame;
    Mat m_eye;

	VideoCapture* m_cap;
	Mat m_eye_window;
	Size cap_size;
	RotFlag cap_rot;

    CascadeClassifier* m_eye_classifier;
    CascadeClassifier* m_face_classifier;

    std::vector<Rect>* m_face_detected_object;
    std::vector<Rect>* m_eye_detected_object;
	Rect m_reg_eye;

    // Used by Eye Image saving and template loading stage
    // To loading newest Eye template image or Save new Eye Image and update Loading template object
    int eye_template_image_count;
    int eye_template_image_useId;


	Point minLoc;


	static EyeDetector* self;

	unsigned proc_count;

	bool is_datafile_open;
    bool isEnableBlinkDetect;
    bool isEnableFaceDetect;

	fstream app_blink_data;

private:
    // Coding to here
    QTime time_stamp;
//	LARGE_INTEGER tStart, tEnd, ts;


public:

	bool record_data(int data_id);
	bool open_recordfile(int data_id);
    void save_eyeimage();
    bool getIsEnableBlinkDetect() const;
};

