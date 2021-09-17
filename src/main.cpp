#include <iostream>
#include <string>

#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>

#include "camera_calibration/camera_calibration.h"
#include "main.h"


std::string keys =
    "{help h      |            | print help                                                                                    }"
	"{@board      | 0          | calibration board type [chessboard: 0, symmetric circle grid: 1, asymmetric circle grid: 2]   }"
	"{@length     | 6          | calibration board length                                                                      }"
	"{@width      | 4          | calibration board width                                                                       }"
	"{@dist       | 0.0265     | distance bettwen points in meters                                                             }"
	"{@path       | .          | calibration parameters file path                                                              }"
    "{source      | 0          | video source path                                                                             }"
    "{count       | 15         | required calibration images count                                                             }"
;


int main(int argc, char* argv[]) {
	cv::CommandLineParser parser(argc, argv, keys);
	parser.about("Сamera calibration v1.0.0");
    if (parser.has("help")) {
        parser.printMessage();
        return Status::GOOD;
    }
	
	const int board_type = parser.get<int>(0);
	const int board_length = parser.get<int>(1);
	const int board_width = parser.get<int>(2);
	const float distance_between_points = parser.get<float>(3);
	const int calibration_images_count = parser.get<int>("count");

	const std::string video_source = parser.get<std::string>("source");
	const std::string calibration_parameters_file = parser.get<std::string>(4);

	if (!parser.check()) {
        parser.printErrors();
        return Status::BAD;
    }

	cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
	cv::Mat distortion_coefficients;

	const cv::Size calibration_board_size = cv::Size(board_length, board_width);
	std::vector<cv::Mat> calibration_images;

	cv::VideoCapture cap;
	if (video_source == "0"){
		cap.open(0);
	} else {
		cap.open(video_source);
	}
	if (!cap.isOpened()) {
		std::cout << " - Couldn't open video capture." << std::endl;
		return Status::BAD;
	}

	cv::namedWindow(kMainWindow, cv::WINDOW_AUTOSIZE);
	
	bool active { true };
	while (active) {
		cv::Mat frame;
		std::vector<cv::Vec2f> found_points;
		cv::Mat draw_frame;

		if (!cap.read(frame)) {
			std::cout << " - Couldn't read frame." << std::endl;
			break;
		}

		bool pattern_found = cv::findChessboardCorners(
			frame, 
			calibration_board_size, 
			found_points, 
			cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE
		);

		if (pattern_found) {
			frame.copyTo(draw_frame);
			cv::drawChessboardCorners(draw_frame, calibration_board_size, found_points, pattern_found);
			cv::imshow(kMainWindow, draw_frame);
		} else {
			cv::imshow(kMainWindow, frame);
		}
		
		char key = cv::waitKey(1000 / FPS);
		switch (key) {
		case Button::SPACE:
			if (pattern_found) {
				cv::Mat temp;
				frame.copyTo(temp);
				calibration_images.push_back(temp);
				std::cout << " - Image saved [images count: " << calibration_images.size() << "]." << std::endl;
			} else {
				std::cout << " - Could not save image [pattern not pattern_found]. " << std::endl;
			}
			break;

		case Button::ENTER:
			if (calibration_images.size() >= calibration_images_count) {
				CameraCalibration camera_calibration(
					calibration_images, 
					calibration_board_size, 
					distance_between_points,
					CameraCalibration::CalibrationGridPattern::CHESSBOARD
				);
				std::cout << " - Calibration completed." << std::endl;

				camera_calibration.SaveCalibrationParameters(calibration_parameters_file);
				std::cout << " - Calibrtion parameters saved [" << calibration_parameters_file << "]." << std::endl;

			} else {
				std::cout << " - Not enough images for calibration [required amount: " << calibration_images_count << "]." << std::endl;
			}
			break;

		case Button::ESC:
			active = false;
			break;
		}
	}



	return Status::GOOD;
}