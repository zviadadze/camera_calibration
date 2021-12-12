#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>

#include "nlohmann/json.hpp"

#include "camera_calibration/camera_calibration.h"

#include "secondary_structures_and_literals.h"


int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv, kKeys);
	parser.about("Сamera calibration v1.0.0");

	if (!parser.has("help") && !parser.has("create") && !parser.has("read")) {
        parser.printMessage();
		return ExitStatus::FAILURE;
	}

    if (parser.has("help")) {
        parser.printMessage();
    }

	camera_calibration::CameraCalibrationSettings settings;
	std::string settings_file_path;
    nlohmann::json settings_json;

	if (parser.has("create")) {
        settings_file_path = parser.get<std::string>("create");

		try {
            camera_calibration::CameraCalibrationSettingsHandler::CreateSettingsFile();
            std::cout << " - Calibration settings file has been created." << std::endl;
        }
        catch(const camera_calibration::CameraCalibrationExeption& excpt) {
            std::cout << excpt.what() << std::endl << std::endl;
            std::cout << " - Session ended." << std::endl;
			return ExitStatus::FAILURE;
        }

        std::cout << " - Start camera calibration? 1 (yes) / 0 (no): ";
        bool start_calibration { false };
        std::cin >> start_calibration;
        if (!start_calibration) {
            std::cout << " - Session ended." << std::endl;
            return ExitStatus::SUCCESS;
        }
    }

    if (parser.has("read")) {
        settings_file_path = parser.get<std::string>("read");
    }

    try {
        std::ifstream fin(settings_file_path);
        fin >> settings_json;
        fin.close();
    }
    catch (nlohmann::json::parse_error excpt) {
        std::cout << " - Unable to get calibration settings from specified file." << std::endl;
        std::cout << " - Session ended." << std::endl;
        return ExitStatus::FAILURE;
	}

    try {
        settings = 
            camera_calibration::CameraCalibrationSettingsHandler::GetSettingsFromJson(settings_json);
        std::cout << " - Calibration settings has been initialized." << std::endl;
    }
    catch (const camera_calibration::CameraCalibrationExeption& excpt) {
        std::cout << excpt.what() << std::endl << std::endl;
        std::cout << " - Session ended." << std::endl;
        return ExitStatus::FAILURE;
    }

    int required_minimum_image_number { 15 };

    if (parser.has("number")) {
        required_minimum_image_number = parser.get<int>("number");;
    }

	if (!parser.check()) {
        parser.printErrors();
        std::cout << " - Session ended." << std::endl;
        return ExitStatus::FAILURE;
    }

    
    std::vector<cv::Mat> calibration_images;
    std::vector<cv::String> calibration_image_names(required_minimum_image_number);
    cv::String image_source_type { settings.GetImageSourceType() };
    cv::String image_source_path { settings.GetImageSourcePath() };
    int calibration_image_count { 0 };

    if (image_source_type == "stream") {
        cv::VideoCapture cap;
        cv::namedWindow(kMainWindowName);

        try {
            cap.open(image_source_path);
        }
        catch(const std::exception& excpt) {
            std::cout << " - Unable to open specified image source." << std::endl;
            std::cout << " - Session ended." << std::endl;
            return ExitStatus::FAILURE;
        }

        bool stop_stream { false };
        bool pattern_found { false };

        while (!stop_stream) {
            cv::Mat image;
            cv::Mat draw_image;
            std::vector<cv::Vec2f> found_points;

            if (!cap.read(image)) {
                std::cout << " - Unable to read image from source." << std::endl;
                std::cout << " - Session ended." << std::endl;
                return ExitStatus::FAILURE;
            }
            
            pattern_found = cv::findChessboardCorners(image, 
                    settings.GetCalibrationBoardSize(), 
                    found_points, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
            
            if (pattern_found) {
                image.copyTo(draw_image);
                cv::drawChessboardCorners(draw_image, settings.GetCalibrationBoardSize(), 
                    found_points, pattern_found);
                cv::imshow(kMainWindowName, draw_image);
            } else {
                cv::imshow(kMainWindowName, image);
            }

            char key = cv::waitKey(1000 / kFPS);
            switch (key) {
            case Button::SPACE :
                if (pattern_found) {
                    calibration_images.push_back(image.clone());
                    ++calibration_image_count;
                    std::cout << " - Calibration image has been accepted [calibration image number: " << 
                        calibration_image_count << "]." << std::endl;
                } 
                else {
                    std::cout << " - Unable to accept image - pattern was not found. " << std::endl;
                }
                break;

            case Button::ENTER :
                if (calibration_image_count >= required_minimum_image_number) {
                    stop_stream = true;
                }
                else {
                    std::cout << " - Insufficient number of calibration images. Required number: " << 
                        required_minimum_image_number << '.' << std::endl;
                }
                break;

            case Button::ESC :
                stop_stream = true;
                break;
            }
        }
    }
    else {
        try {
            cv::glob(image_source_path, calibration_image_names);

            if (calibration_image_names.size() < required_minimum_image_number) {
                std::cout << " - Insufficient number of calibration images. Required number: " << 
                        required_minimum_image_number << '.' << std::endl;
                return ExitStatus::FAILURE;
            }

            for (int i { 0 }; i < calibration_image_names.size(); ++i) {
                cv::Mat image = cv::imread(calibration_image_names[i]);
                calibration_images.push_back(image.clone());
            }
        }
        catch(const std::exception& excpt) {
            std::cout << " - Unable to open specified image source." << std::endl;
            std::cout << " - Session ended." << std::endl;
            return ExitStatus::FAILURE;
        }
    }


    std::cout << " - Camera calibration has started. " << std::endl;

    camera_calibration::CameraCalibration(settings, calibration_images).
        ExtractCameraParameters().SaveToFile(settings.GetCameraParametersFilePath());

    std::cout << " - Camera calibration has been completed. " << std::endl;
    std::cout << " - Calibration parameters saved to: " << settings.GetCameraParametersFilePath() << std::endl;
    std::cout << " - Session ended." << std::endl;

    return ExitStatus::SUCCESS;
}