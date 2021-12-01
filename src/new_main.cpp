#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>

#include "nlohmann/json.hpp"

#include "camera_calibration/camera_calibration.h"

#include "main_structures.h"
#include "main_functions.h"


int main()
{
    nlohmann::json camera_calibration_parameters_json;
    camera_calibration::CameraCalibrationParameters camera_calibration_parameters(camera_calibration_parameters_json);

    std::vector<cv::Mat> calibration_images;
    camera_calibration::CameraCalibration camera_calibration(camera_calibration_parameters, calibration_images);

    std::string filename = "camera_parameters.txt";
    camera_calibration.ExtractCameraParameters().SaveToFile(filename);

    return 0;
}