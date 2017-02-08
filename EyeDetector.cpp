#include "EyeDetector.h"
#include <stdlib.h>
#include <stdio.h>

EyeDetector* EyeDetector::self;

EyeDetector::EyeDetector() :
	cap_size(640, 480),
	m_cap(nullptr),
	cap_rot(RotFlag::ROT_0),
    m_eye_classifier(nullptr),
    m_face_classifier(nullptr),
	proc_count(0),
    is_datafile_open(false),
    isEnableBlinkDetect(false),
    isEnableFaceDetect(true),
    eye_template_image_count(0),
    eye_template_image_useId(0) {

    pending_mutex.unlock();

	// Setup the Window property
//	namedWindow(WINDOW_NAME_ORIGIN_IMAGE, WINDOW_AUTOSIZE);
//	namedWindow(WINDOW_NAME_MODIFY_IMAGE, WINDOW_AUTOSIZE);
//	namedWindow(WINDOW_NAME_EYE, WINDOW_AUTOSIZE);

	// Relocation the Window
//	moveWindow(WINDOW_NAME_ORIGIN_IMAGE, 10, 10);
//	moveWindow(WINDOW_NAME_MODIFY_IMAGE, 360+10, 10);
//	moveWindow(WINDOW_NAME_EYE, 360 + 10 + 360 + 10, 10);
}


EyeDetector::~EyeDetector() {
	if (app_blink_data.is_open()) app_blink_data.close();
}

void EyeDetector::init(VideoCapture &cap) {
	m_cap = &cap;

	if (!m_cap->isOpened()) {

		reportStatus(EyeDetectorStatus::NO_CAMERA_AVAILABLE, "Not open");

	} else {
		char cap_info[STATUS_MSG_SIZE] = { 0 };
        sprintf(cap_info, "Width: %f,  Height: %f",
			cap.get(CAP_PROP_FRAME_WIDTH),
			cap.get(CAP_PROP_FRAME_HEIGHT));

		reportStatus(EyeDetectorStatus::CAMERA_OPENED, cap_info);
	}

	self = this;

	return;
}

void EyeDetector::init(VideoCapture& cap, Size frameSize) {
	cap_size = frameSize;
	init(cap);
}

void EyeDetector::init(VideoCapture& cap, Size frameSize, RotFlag rotate) {
	cap_size = frameSize;
	cap_rot = rotate;
	init(cap);
}

bool EyeDetector::update_image() {

	if (!m_cap->isOpened()) {
		reportStatus(EyeDetectorStatus::NO_CAMERA_AVAILABLE, "Can't get a frame");
		return false;
	}

    bool result(true);
    result = m_cap->read(m_frame);

    // Check there is successful read image
    if(!result) return false;

	resize(m_frame, m_frame, cap_size, 0.0, 0.0, CV_INTER_LINEAR);
	//Mat rot_mat = getRotationMatrix2D(Point2f(cap_size / 2), cap_rotate, 1.0);
	//transpose(frame, frame);
	//flip(frame, frame,1);
	//warpAffine(frame, frame, rot_mat, cap_size);
	rot90(m_frame, cap_rot);
	proc_count++;
	return true;
}

void EyeDetector::show_frame(Mat& modify_frame, Mat& eye_frame) {
	bool isDetected_blink(false);

    //imshow(WINDOW_NAME_ORIGIN_IMAGE, m_frame);
	modify_frame = m_frame.clone();

    Mat gray_frame;
    cvtColor(modify_frame, gray_frame, CV_RGB2GRAY);

    Rect roi_rect;

    if (m_eye_classifier != nullptr) {
		
        bool is_face_detected = detect_face(gray_frame);
        bool is_eye_detected(false);

        if(isEnableFaceDetect) {

            if(m_face_detected_object->size() > 0) {

                // Compute roi-rectangle for eye search Window
                Rect face = m_face_detected_object->front();

                roi_rect = Rect(face.x + face.width/2, face.y + face.height / 8, face.width / 2, face.height * 0.4);

                Mat roi_eye_image(gray_frame, roi_rect);

                rectangle(modify_frame, roi_rect, CV_RGB(0, 0, 255), 2);

                // Process the Detect eye
                is_eye_detected = detect_eye(
                            roi_eye_image,
                            Size(roi_rect.width * 0.3, roi_rect.height * 0.3),
                            Size(roi_rect.width * 0.6, roi_rect.height * 0.6));

                if (is_face_detected)
                    rectangle(modify_frame, m_face_detected_object->front(), CV_RGB(255, 0, 128), 2);
                else {
                    rectangle(modify_frame, m_face_detected_object->front(), CV_RGB(255, 0, 255), 2);
                }
            }

        }
        else {



            is_eye_detected = detect_eye(
                        gray_frame,
                        Size(cap_size.height / 5, cap_size.width / 10),
                        Size(cap_size.height / 2, cap_size.width / 7));
        }



        // Finding most right eye in the frame
        Rect left_eye;
        left_eye.x = 0;
        left_eye.y = 0;
        left_eye.width = 0;
        left_eye.height = 0;
        for(auto eyes : (*m_eye_detected_object)) {
            if(eyes.x > left_eye.x) {
                left_eye = eyes;
                left_eye.x += roi_rect.x;
                left_eye.y += roi_rect.y;
            }

        }

        if (is_eye_detected)
            rectangle(modify_frame, left_eye, CV_RGB(255, 0, 0), 2);
        else {
            rectangle(modify_frame, left_eye, CV_RGB(0, 0, 255), 2);
        }

        // Register the eye frame
        m_eye = Mat(m_frame, left_eye);
        // Register the eye search frame
        Rect eyes_sch_win(left_eye);
        left_eye.width += 40;
        left_eye.height += 40;
        left_eye.x -= 20;
        left_eye.y -= 20;

        // Check the size is ok
        if(left_eye.y <= 0) return;
        if(left_eye.x <= 0) return;

        if(left_eye.y + left_eye.height >= cap_size.width) return;
        if(left_eye.y - left_eye.height >= cap_size.width) return;

        if(left_eye.x + left_eye.width >= cap_size.height) return;
        if(left_eye.x - left_eye.width >= cap_size.height) return;


        m_eye_window = Mat(m_frame, left_eye);
        cvtColor(m_eye_window, m_eye_window, CV_RGB2GRAY);

        Mat hsv_eye;
        cvtColor(m_eye, hsv_eye, CV_RGB2HSV);
        cvtColor(m_eye, m_eye, CV_RGB2GRAY);
        //vector<Mat> hsv_planes;
        //split(hsv_eye, hsv_planes);
        //Mat v_hist;

        ////equalizeHist(hsv_planes[2], v_hist);
        //threshold(hsv_planes[2], v_hist, 30, 255, ThresholdTypes::THRESH_BINARY);
        //imshow("Equalized Image", v_hist);

        /********* Detect Eye *********/
        if(isEnableBlinkDetect)
            isDetected_blink = detect_blink();
        /******************************/

        m_eye_window.copyTo(eye_frame);

        /****** Add the text on image ******/
        char str_buffer[64] = { 0 };
        sprintf(str_buffer, "(%2.2f,%2.2f)",
            (double)cap_size.height/ (double)left_eye.width,
            (double)cap_size.width/ (double)left_eye.height);
        putText(modify_frame, String(str_buffer), Point(left_eye.x, left_eye.y - 5), FONT_HERSHEY_COMPLEX, 0.9, CV_RGB(255,255,255), 2.5,8);
        sprintf(str_buffer, "%fFPS",m_cap->get(CAP_PROP_FPS));
        putText(modify_frame, String(str_buffer), Point(3, 27), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(128, 128, 128), 3);
        putText(modify_frame, String(str_buffer), Point(0, 25), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 0, 0), 3);
        sprintf(str_buffer, "Proc Count %d", proc_count);
        putText(modify_frame, String(str_buffer), Point(3, 50), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(128, 128, 128), 3);
        putText(modify_frame, String(str_buffer), Point(0, 48), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 0, 0), 3);
        sprintf(str_buffer, "Compare value = %f", minVal);
        putText(modify_frame, String(str_buffer), Point(3, 73), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(128, 128, 128), 3);
        putText(modify_frame, String(str_buffer), Point(0, 71), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 0, 0), 3);

        /***********************************/

    }
}

bool EyeDetector::detect_face(Mat& roi_image) {
    vector<Rect> face_new;

    // TODO: Using Face Calssifier to detect face
    m_face_classifier->detectMultiScale(
                roi_image,
                face_new,
                1.1,
                5,
                CV_HAAR_MAGIC_VAL,
                Size(200, 200),
                Size(430, 430));

    if(face_new.size() > 0) {
        // If there found any face
        *m_face_detected_object = face_new;
        return true;
    }

    return false;

}

bool EyeDetector::detect_eye(Mat& roi_image, Size min, Size max) {

	vector<Rect> eye_new;

    m_eye_classifier->detectMultiScale(
        roi_image,
		eye_new,
		1.1,
		5,
		CV_HAAR_MAGIC_VAL,
        min,
        max);


	if (eye_new.size() > 0) {
        *m_eye_detected_object = eye_new;
		return true;
	}
	else {
		return false;
	}
}

void EyeDetector::set_classifier(
        CascadeClassifier &eye_classifier,
        CascadeClassifier &face_classifier,
        std::vector<Rect> &eye_detected_set,
        std::vector<Rect> &face_detected_set ) {

    // Register the Classifier and Results vector
    m_eye_classifier = &eye_classifier;
    m_face_classifier = &face_classifier;
    m_face_detected_object = &face_detected_set;
    m_eye_detected_object = &eye_detected_set;
}

void EyeDetector::set_enable_blink_detect(bool enable)
{
    isEnableBlinkDetect = enable;
}

void EyeDetector::set_enable_face_detect(bool enable)
{
    isEnableFaceDetect = enable;
}

Mat EyeDetector::get_raw_frame()
{
    return m_frame;
}

void EyeDetector::reportStatus(EyeDetectorStatus status) {
	return reportStatus(status, "NULL");
}

void EyeDetector::rot90(Mat &image, RotFlag flag) {
	switch (flag) {
		case RotFlag::ROT_CW_90 :
			transpose(image, image);
			flip(image, image, 1);
			break;
		case RotFlag::ROT_CCW_90 :
			transpose(image, image);
			flip(image, image, 0);
			break;
		case RotFlag::ROT_180 :
			flip(image, image, -1);
		default:
			break;
	}
}

bool EyeDetector::detect_blink() {

	stringstream ss;
    ss << ".//template//temp" << eye_template_image_useId << ".jpg";
	string str;
	ss >> str;

	Mat image_template = imread(str.c_str(),CV_8U);


	// Using Template match
	Mat result;
	//blur(m_eye_window, m_eye_window, Size(10, 10));
	//blur(image_template, image_template, Size(10, 10));

	
	matchTemplate(m_eye_window, image_template, result, CV_TM_SQDIFF_NORMED);

    // Lock the data, inorder to record new min Val
    pending_mutex.lock();
	minMaxLoc(result, &minVal, 0, &minLoc, 0);

	// Using Histgram Compare
	MatND hist_base;
	MatND hist_test;
	MatND error;

	/// Using 128 bins for gray value
	int g_bins = 128;
	int histSize[] = { g_bins };

	float s_ranges[] = { 0, 256 };
	const float* range[] = { s_ranges };

	int channels[] = { 0 };
	calcHist(&image_template, 1, channels, Mat(), hist_base , 1, histSize, range);
	normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

	calcHist(&m_eye, 1, channels, Mat(), hist_test, 1, histSize, range);
	normalize(hist_test, hist_test, 0, 1, NORM_MINMAX, -1, Mat());

	absdiff(hist_base, hist_test, error);

	//double result = compareHist(image_template, m_eye, HistCompMethods::HISTCMP_CORREL);
	Scalar sumError = sum(error);
	absSum = sumError[0];

	if (minVal > 4744717) return true;

    pending_mutex.unlock();

	return false;
}

bool EyeDetector::getIsEnableBlinkDetect() const
{
    return isEnableBlinkDetect;
}

template<size_t STR_SIZE>
void EyeDetector::reportStatus(EyeDetectorStatus status, const char (&metaMsg)[STR_SIZE]) {
	
	// Reset string variable
	char statusMsg[STATUS_MSG_SIZE] = { 0 };

	std::stringstream stream;

	stream.rdbuf()->pubsetbuf(statusMsg, sizeof(statusMsg));

	stream << "Eye Detector [Status]:\t";

	switch (status) {
		case EyeDetectorStatus::CAMERA_CLOSED:
			stream << "Camera was closed";
			break;
		case EyeDetectorStatus::CAMERA_OPENED:
			stream << "Camera was opened";
			break;
		case EyeDetectorStatus::CAPTURE_EYE:
			stream << "Eyes Capture";
			break;
		case EyeDetectorStatus::NO_CAMERA_AVAILABLE:
			stream << "No camera available";
			break;
		default:
			break;
	}
	stream << "\t <" << metaMsg << ">" ;


	string str;
	getline(stream, str);


	cout << str << endl;
	
	return;
}


//void EyeDetector::doMemberMouseCallback(int aEvent, int x, int y, int flags) {
//	static int image_count(0);
//	if (aEvent != EVENT_LBUTTONDOWN)
//		return;

//	save_eyeimage(image_count++);
//}


bool EyeDetector::record_data(int data_id) {
	if (!is_datafile_open) {
		is_datafile_open = open_recordfile(data_id);
		return false;
	}

	// Get past time
    float m_time = static_cast<float>(time_stamp.elapsed()) / 1000.0;

    pending_mutex.lock();

	app_blink_data
		<< setw(15) << setprecision(6) << m_time
		<< setw(15) << setprecision(6) << absSum
		<< endl;

    pending_mutex.unlock();


}


bool EyeDetector::open_recordfile(int data_id) {
	stringstream ss;
	ss << ".//data//data" << data_id << ".txt";
	string str;
	ss >> str;

	app_blink_data.open(str.c_str(), ios::out);
	
	if (app_blink_data.is_open()) {
        time_stamp.start();
		return true;
	}
	
	return false;
}


void EyeDetector::save_eyeimage() {

    eye_template_image_useId = eye_template_image_count;
	stringstream ss;
    ss << ".//template//temp" << eye_template_image_count++ << ".jpg";
	string str;
	ss >> str;

	imwrite(str.c_str(), m_eye);
}
