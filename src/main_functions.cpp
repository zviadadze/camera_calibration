#include <iostream>
#include <fstream>
#include <string>

#include "main_functions.h"


bool CreateCalibrationSettingsFile(const std::string &calibration_settings_file) {
    std::string camera_parameters_file_path;
    std::string video_source;
    int required_images_count;
    int board_type;
    int board_length;
    int board_width;
    float distance_between_points;
    std::string circle_detector_settings_path;
    
    std::cout << " - Enter following fields ->" << std::endl;

    std::cout << " Calibration parameters file path: ";
    std::cin >> camera_parameters_file_path;

    std::cout << " Video source path: ";
    std::cin >> video_source;

    std::cout << " Reqired calibration images count: ";
    std::cin >> required_images_count;

    std::cout << " Calibration board type (CHESSBOARD: 0, SYMMETRCI_CIRCLE_GRID: 1, ASSYMMETRIC_CIRCLE_GRID: 2): ";
    std::cin >> board_type;

    std::cout << " Calibration board length: ";
    std::cin >> board_length;

    std::cout << " Calibration board width: ";
    std::cin >> board_width;

    std::cout << " Distance between points: ";
    std::cin >> distance_between_points;

    if(board_type == 1 || board_type == 2) {
        std::cout << " Circle detector settings path: ";
        std::cin >> circle_detector_settings_path;
    } else {
        circle_detector_settings_path = "empty";
    }

    nlohmann::json calibration_parameters = {
        {"camera_parameters_file_path", camera_parameters_file_path},
        {"video_source", video_source},
        {"required_images_count", required_images_count},
        {"board_type", board_type},
        {"board_length", board_length},
        {"board_width", board_width},
        {"distance_between_points", distance_between_points},
        {"circle_detector_settings_path", circle_detector_settings_path}
    };

    std::ofstream fout(calibration_settings_file);
    fout << calibration_parameters << std::endl;
    fout.close();

    return true;
}

bool GetCalibrationSettings(
    const nlohmann::json &calibration_settings_json, 
    CalibrationSettings &calibration_settings
) {
    calibration_settings.camera_parameters_file_path = calibration_settings_json["camera_parameters_file_path"].get<std::string>();
    calibration_settings.video_source = calibration_settings_json["video_source"].get<std::string>();
    calibration_settings.required_images_count = calibration_settings_json["required_images_count"].get<int>();
    calibration_settings.board_type = calibration_settings_json["board_type"].get<int>();
    calibration_settings.board_length = calibration_settings_json["board_length"].get<int>();
    calibration_settings.board_width = calibration_settings_json["board_width"].get<int>();
    calibration_settings.distance_between_points = calibration_settings_json["distance_between_points"].get<float>();
    calibration_settings.circle_detector_settings_path = calibration_settings_json["circle_detector_settings_path"].get<std::string>();

    return true;
}

bool GetCircleDetectorSettings(
    const nlohmann::json circle_detector_settings_json,
    cv::SimpleBlobDetector::Params &circle_detector_settings
) {
    circle_detector_settings.maxArea = circle_detector_settings_json["maxArea"].get<int>();
	circle_detector_settings.minArea = circle_detector_settings_json["minArea"].get<int>();
	circle_detector_settings.minThreshold = circle_detector_settings_json["minThreshold"].get<int>();
	circle_detector_settings.maxThreshold = circle_detector_settings_json["maxThreshold"].get<int>();
	circle_detector_settings.thresholdStep = circle_detector_settings_json["thresholdStep"].get<int>();
	circle_detector_settings.filterByCircularity = circle_detector_settings_json["filterByCircularity"].get<bool>();
	circle_detector_settings.minCircularity = circle_detector_settings_json["minCircularity"].get<float>();
	circle_detector_settings.filterByInertia = circle_detector_settings_json["filterByInertia"].get<bool>();
	circle_detector_settings.filterByConvexity = circle_detector_settings_json["filterByConvexity"].get<bool>();
}
