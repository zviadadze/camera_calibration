#ifndef MAIN_STRUCTURES_H_
#define MAIN_STRUCTURES_H_

#include<opencv2/calib3d.hpp>

#include "nlohmann/json.hpp"

#include "main_structures.h"


bool CreateCalibrationSettingsFile(const std::string &calibration_settings_file);

bool GetCalibrationSettings(
    const nlohmann::json &calibration_settings_json, 
    CalibrationSettings &calibration_settings
);

bool GetCircleDetectorSettings(
    const nlohmann::json circle_detector_settings_json,
    cv::SimpleBlobDetector::Params &circle_detector_settings
);


#endif