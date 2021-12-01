#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>

#include "nlohmann/json.hpp"

#include "camera_calibration/camera_calibration.h"

#include "main_structures.h"
#include "main_functions.h"


const std::string kKeys {
    "{help h   |        | print help                                           }"
	"{create c | <none> | create calibration settings file (json)              }"
	"{read r   | <none> | read calibration settings from specified file (json) }" 
};

std::string kMainWindow { "Main" };

const int FPS { 25 };


int main(int argc, char* argv[]) {
	// cv::CommandLineParser parser(argc, argv, kKeys);
	// parser.about("Сamera calibration v1.0.0");

	// if (!parser.has("help") && !parser.has("create") && !parser.has("read")) {
	// 	return Status::GOOD;
	// }

    // if (parser.has("help")) {
    //     parser.printMessage();
    //     return Status::GOOD;
    // }

	// std::string calibration_settings_file;
	// CalibrationSettings calibration_settings;

	// if (parser.has("create")) {
    //     calibration_settings_file = parser.get<std::string>("create");

	// 	std::cout << calibration_settings_file << std::endl;
	// 	if (CreateCalibrationSettingsFile(calibration_settings_file)) {
	// 		std::cout << " - Calibration settings file successfully created." << std::endl;
    //     	return Status::GOOD;
	// 	} else {
	// 		std::cout << " - Couldn't create calibration settings file." << std::endl;
	// 		return Status::BAD;
	// 	}
    // }

	// if (parser.has("read")) {
    //     calibration_settings_file = parser.get<std::string>("read");
		
	// 	std::ifstream fin(calibration_settings_file);
	// 	nlohmann::json calibration_settings_json; 
	// 	fin >> calibration_settings_json;
	// 	fin.close();

	// 	if (!GetCalibrationSettings(calibration_settings_json, calibration_settings)) {
	// 		std::cout << " - Couldn't get calibration settings from specified file." << std::endl;
	// 		return Status::BAD;
	// 	}
    // }

	// if (!parser.check()) {
    //     parser.printErrors();
    //     return Status::BAD;
    // }


	// cv::SimpleBlobDetector::Params circle_detector_settings;
	// cv::Ptr<cv::SimpleBlobDetector> circle_detector;

	// if (calibration_settings.board_type == BoardType::SYMMETRIC_CIRCLE_GRID || 
	// 	calibration_settings.board_type == BoardType::ASYMMETRIC_CIRCLE_GRID) {
	// 	if (calibration_settings.circle_detector_settings_path != "empty") {
	// 		std::cout << " - Circle detector settings path is not specified." << std::endl;
	// 		return Status::BAD;
	// 	}

	// 	std::ifstream fin(calibration_settings.circle_detector_settings_path);
	// 	nlohmann::json circle_detector_settings_json;
	// 	fin >> circle_detector_settings_json;
	// 	fin.close();

	// 	GetCircleDetectorSettings(
	// 		circle_detector_settings_json,
	// 		circle_detector_settings
	// 	);
	// 	circle_detector = cv::SimpleBlobDetector::create(circle_detector_settings);
	// }

	// cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
	// cv::Mat distortion_coefficients;

	// const cv::Size calibration_board_size = cv::Size(calibration_settings.board_length, calibration_settings.board_width);
	// std::vector<cv::Mat> calibration_images;

	// cv::VideoCapture cap;
	// // "0" equals webcam  
	// if (calibration_settings.video_source == "0") {
	// 	cap.open(0);
	// } else {
	// 	cap.open(calibration_settings.video_source);
	// }
	// if (!cap.isOpened()) {
	// 	std::cout << " - Couldn't open video capture." << std::endl;
	// 	return Status::BAD;
	// }

	// cv::namedWindow(kMainWindow, cv::WINDOW_AUTOSIZE);
	
	// bool active { true };
	// while (active) {
	// 	cv::Mat frame;
	// 	std::vector<cv::Vec2f> found_points;
	// 	cv::Mat draw_frame;

	// 	if (!cap.read(frame)) {
	// 		std::cout << " - Couldn't read frame." << std::endl;
	// 		break;
	// 	}

	// 	bool pattern_found {false};

	// 	switch (calibration_settings.board_type) {
	// 	case BoardType::CHESSBOARD:
	// 		pattern_found = cv::findChessboardCorners(
	// 			frame, 
	// 			calibration_board_size, 
	// 			found_points, 
	// 			cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE
	// 		);
	// 		break;

	// 	case BoardType::SYMMETRIC_CIRCLE_GRID:
	// 		pattern_found = cv::findCirclesGrid(
	// 			frame,
	// 			calibration_board_size,
	// 			found_points,
	// 			cv::CALIB_CB_SYMMETRIC_GRID,
	// 			circle_detector
	// 		);
	// 		break;
		
	// 	case BoardType::ASYMMETRIC_CIRCLE_GRID:

	// 		pattern_found = cv::findCirclesGrid(
	// 			frame,
	// 			calibration_board_size,
	// 			found_points,
	// 			cv::CALIB_CB_SYMMETRIC_GRID,
	// 			circle_detector
	// 		);
	// 		break;

	// 	default:
	// 		std::cout << " - Unsupported caibration board type index: " << calibration_settings.board_type << '.' << std::endl;
	// 		std::cout << " [use: CHESSBOARD: 0, SYMMETRCI_CIRCLE_GRID: 1, ASSYMMETRIC_CIRCLE_GRID: 2]" << std::endl;
	// 		return Status::BAD;
	// 	} 

	// 	if (pattern_found) {
	// 		frame.copyTo(draw_frame);
	// 		cv::drawChessboardCorners(draw_frame, calibration_board_size, found_points, pattern_found);
	// 		cv::imshow(kMainWindow, draw_frame);
	// 	} else {
	// 		cv::imshow(kMainWindow, frame);
	// 	}
		
	// 	char key = cv::waitKey(1000 / FPS);
	// 	switch (key) {
	// 	case Button::SPACE:
	// 		if (pattern_found) {
	// 			cv::Mat temp;
	// 			frame.copyTo(temp);
	// 			calibration_images.push_back(temp);
	// 			std::cout << " - Image saved [images count: " << calibration_images.size() << "]." << std::endl;
	// 		} else {
	// 			std::cout << " - Could not save image [pattern not pattern_found]. " << std::endl;
	// 		}
	// 		break;

	// 	case Button::ENTER:
	// 		if (calibration_images.size() >= calibration_settings.required_images_count) {
	// 			CameraCalibration camera_calibration(
	// 				calibration_images, 
	// 				calibration_board_size, 
	// 				calibration_settings.distance_between_points,
	// 				CameraCalibration::CalibrationGridPattern::CHESSBOARD
	// 			);
	// 			std::cout << " - Calibration completed." << std::endl;

	// 			camera_calibration.SaveCalibrationParameters(calibration_settings.camera_parameters_file_path);
	// 			std::cout << " - Calibration parameters saved [" << calibration_settings.camera_parameters_file_path << "]." << std::endl;

	// 		} else {
	// 			std::cout << " - Not enough images for calibration [required amount: " << 
	// 				calibration_settings.required_images_count << "]." << std::endl;
	// 		}
	// 		break;

	// 	case Button::ESC:
	// 		active = false;
	// 		break;
	// 	}
	// }



	return Status::GOOD;
}