#include<opencv2/core.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/calib3d.hpp>

#include <sstream>
#include <iostream>
#include <fstream>

#include "camera_calibration/camera_calibration.h"


const float CALIBRATION_SQUARE_DIMENSION = 0.0265f; //длина cтороны квадрата на калибровочной доcке (в метрах)
const cv::Size CHESSBOARD_DIMENSSIONS = cv::Size(6, 4); // Размерноcть калибровочной доcки (количеcтво квадратов в длину и ширину cоответcтвенно)

const int FPS = 25;
const int CALIBRATION_IMAGES_COUNT = 15;
const std::string MAIN_WINDOW = "Webcam";
const std::string CAMERA_CALIBRATION_PARAMETERS_FILENAME = "calibration_parameters";


int main() {
	std::vector<cv::Mat> calibration_images;
	cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
	cv::Mat distortion_coefficients;
	
	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		std::cout << " - Couldn't open Video Capture." << std::endl;
		return 0;
	} else {
		std::cout << " - Video Capture successfully opened." << std::endl;
	}

	cv::namedWindow(MAIN_WINDOW, cv::WINDOW_AUTOSIZE);

	bool active = true;
	while (active) {
		cv::Mat frame;
		cv::Mat draw_frame;
		std::vector<cv::Vec2f> found_corners;

		if (!cap.read(frame)) {
			std::cout << " - Couldn't read frame." << std::endl;
			break;
		}

		bool found = cv::findChessboardCorners(frame, CHESSBOARD_DIMENSSIONS, found_corners, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
		if (found) {
			frame.copyTo(draw_frame);
			cv::drawChessboardCorners(draw_frame, CHESSBOARD_DIMENSSIONS, found_corners, found);
			cv::imshow(MAIN_WINDOW, draw_frame);
		} else {
			cv::imshow(MAIN_WINDOW, frame);
		}
		
		char key = cv::waitKey(1000 / FPS);
		switch (key) {
		case ' ':
			if (found) {
				cv::Mat temp;
				frame.copyTo(temp);
				calibration_images.push_back(temp);
				std::cout << " - Image saved [images count: " << calibration_images.size() << "]." << std::endl;
			}
			break;
		case 13:
			if (calibration_images.size() >= CALIBRATION_IMAGES_COUNT) {
				CameraCalibration camera_calibration(calibration_images, CHESSBOARD_DIMENSSIONS, CALIBRATION_SQUARE_DIMENSION);
				camera_calibration.SaveCameraCalibrationParameters(CAMERA_CALIBRATION_PARAMETERS_FILENAME);
				camera_calibration.LoadCameraCalibrationParameters(CAMERA_CALIBRATION_PARAMETERS_FILENAME);
			} else {
				std::cout << " - Not enough images for calibration [required images count: " << CALIBRATION_IMAGES_COUNT << "]." << std::endl;
			}
			break;
		case 27:
			active = false;
			break;
		}
	}

	return 0;
}