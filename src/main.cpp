#include <iostream>

#include <opencv2/highgui.hpp>

#include "camera_calibration/camera_calibration.h"
#include "main.h"


// CHESSBOARD EXAMPLE

// int main() {
// 	std::vector<cv::Mat> calibration_images;
// 	cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
// 	cv::Mat distortion_coefficients;

	
// 	cv::VideoCapture cap(VideoSource::WEBCAM);
// 	if (!cap.isOpened()) {
// 		std::cout << " - Couldn't open Video Capture." << std::endl;
// 		return -1;
// 	}

// 	cv::namedWindow(kMainWindow, cv::WINDOW_AUTOSIZE);

// 	bool active { true };
// 	while (active) {
// 		cv::Mat frame;
// 		cv::Mat draw_frame;
// 		std::vector<cv::Vec2f> found_points;

// 		if (!cap.read(frame)) {
// 			std::cout << " - Couldn't read frame." << std::endl;
// 			break;
// 		}

// 		bool found = cv::findChessboardCorners(
// 			frame, 
// 			kCalibrationBoardSize, 
// 			found_points, 
// 			cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE
// 		);
// 		if (found) {
// 			frame.copyTo(draw_frame);
// 			cv::drawChessboardCorners(draw_frame, kCalibrationBoardSize, found_points, found);
// 			cv::imshow(kMainWindow, draw_frame);
// 		} else {
// 			cv::imshow(kMainWindow, frame);
// 		}
		
// 		char key = cv::waitKey(1000 / FPS);
// 		switch (key) {
// 		case Buttons::SPACE:
// 			if (found) {
// 				cv::Mat temp;
// 				frame.copyTo(temp);
// 				calibration_images.push_back(temp);
// 				std::cout << " - Image saved [images count: " << calibration_images.size() << "]." << std::endl;
// 			} else {
// 				std::cout << " - Could not save image [pattern not found]. " << std::endl;
// 			}
// 			break;
// 		case Buttons::ENTER:
// 			if (calibration_images.size() >= kCalibrationImagesCount) {
// 				CameraCalibration camera_calibration(
// 					calibration_images, 
// 					kCalibrationBoardSize, 
// 					kDistanceBetweenPoints
// 				);
// 				camera_matrix = camera_calibration.GetCameraMatrix();
// 				distortion_coefficients = camera_calibration.GetDistortionCoefficients();
// 				camera_calibration.SaveCalibrationParameters(kCalibrationParametersFilename);
// 			} else {
// 				std::cout << " - Not enough images for calibration [required amount: " << kCalibrationImagesCount << "]." << std::endl;
// 			}
// 			break;
// 		case Buttons::ESC:
// 			active = false;
// 			break;
// 		}
// 	}

// 	return OK;
// }


// CIRCLE GRID EXAMPLE

int main() {
	std::vector<cv::Mat> calibration_images;
	cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
	cv::Mat distortion_coefficients;

	cv::SimpleBlobDetector::Params circle_detector_params;
	circle_detector_params.maxArea = 30000;
	circle_detector_params.minArea = 1000;
	circle_detector_params.minThreshold = 10;
	circle_detector_params.maxThreshold = 180;
	circle_detector_params.thresholdStep = 5;
	circle_detector_params.filterByCircularity = true;
	circle_detector_params.minCircularity = 0.7;
	circle_detector_params.filterByInertia = false;
	circle_detector_params.filterByConvexity = false;

	cv::Ptr<cv::SimpleBlobDetector> circle_detector = 
		cv::SimpleBlobDetector::create(circle_detector_params);
	
	cv::VideoCapture cap(VideoSource::WEBCAM);
	if (!cap.isOpened()) {
		std::cout << " - Couldn't open Video Capture." << std::endl;
		return -1;
	}

	cv::namedWindow(kMainWindow, cv::WINDOW_AUTOSIZE);

	bool active { true };
	while (active) {
		cv::Mat frame;
		cv::Mat draw_frame;
		std::vector<cv::Vec2f> found_points;

		if (!cap.read(frame)) {
			std::cout << " - Couldn't read frame." << std::endl;
			break;
		}

		bool found = cv::findCirclesGrid(
			frame, 
			kCalibrationBoardSize, 
			found_points, 
			cv::CALIB_CB_ASYMMETRIC_GRID,
			circle_detector
		);
		if (found) {
			frame.copyTo(draw_frame);
			cv::drawChessboardCorners(draw_frame, kCalibrationBoardSize, found_points, found);
			cv::imshow(kMainWindow, draw_frame);
		} else {
			cv::imshow(kMainWindow, frame);
		}
		
		char key = cv::waitKey(1000 / FPS);
		switch (key) {
		case Buttons::SPACE:
			if (found) {
				cv::Mat temp;
				frame.copyTo(temp);
				calibration_images.push_back(temp);
				std::cout << " - Image saved [images count: " << calibration_images.size() << "]." << std::endl;
			} else {
				std::cout << " - Could not save image [pattern not found]. " << std::endl;
			}
			break;
		case Buttons::ENTER:
			if (calibration_images.size() >= kCalibrationImagesCount) {
				CameraCalibration camera_calibration(
					calibration_images, 
					kCalibrationBoardSize, 
					kDistanceBetweenPoints,
					circle_detector,
					CameraCalibration::CirclePatternType::ASYMMETRIC
				);
				camera_matrix = camera_calibration.GetCameraMatrix();
				distortion_coefficients = camera_calibration.GetDistortionCoefficients();
				camera_calibration.SaveCalibrationParameters(kCalibrationParametersFilename);
			} else {
				std::cout << " - Not enough images for calibration [required amount: " << kCalibrationImagesCount << "]." << std::endl;
			}
			break;
		case Buttons::ESC:
			active = false;
			break;
		}
	}

	return OK;
}