#ifndef MAIN_H_
#define MAIN_H_

#include <opencv2/core/types.hpp>
#include <string>


enum Button {
	SPACE = 32,
	ENTER = 13,
	ESC = 27
};

enum BoardType {
	CHESSBOARD = 0,
	SYMMETRIC_CIRCLE_GRID = 1,
	ASYMMETRIC_CIRCLE_GRID = 2
};

enum Status {
	GOOD = 0,
	BAD = -1
};

struct CalibrationSettings {
	std::string calibration_settings_file;
	std::string camera_parameters_file_path;
	std::string video_source;
	int required_images_count; 
	int board_type;
	int board_length;
	int board_width;
	float distance_between_points;
	std::string circle_detector_settings_path;
};


#endif
