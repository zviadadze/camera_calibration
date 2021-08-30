#include <iostream>

#include <opencv2/highgui.hpp>

#include "camera_calibration/camera_calibration.h"
#include "main.h"


int main() {
	std::vector<cv::Mat> calibration_images;
	cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
	cv::Mat distortion_coefficients;
	
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
		std::vector<cv::Vec2f> found_corners;

		if (!cap.read(frame)) {
			std::cout << " - Couldn't read frame." << std::endl;
			break;
		}

		bool found = cv::findChessboardCorners(frame, kChessboardDimensions, found_corners, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
		if (found) {
			frame.copyTo(draw_frame);
			cv::drawChessboardCorners(draw_frame, kChessboardDimensions, found_corners, found);
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
				CameraCalibration camera_calibration(calibration_images, kChessboardDimensions, kCalibrationSquareLength);
				camera_calibration.SaveCalibrationParameters(kCalibrationParametersFilename);
			} else {
				std::cout << " - Not enough images for calibration [required images count: " << kCalibrationImagesCount << "]." << std::endl;
			}
			break;
		case Buttons::ESC:
			active = false;
			break;
		}
	}

	return OK;
}