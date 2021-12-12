#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "camera_calibration/camera_calibration.h"

namespace camera_calibration {


cv::Mat CameraParameters::GetCameraMatrix() const { return camera_matrix_; }
cv::Mat CameraParameters::GetDistrotionCoefficients() const { return distortion_coefficients_; }
std::vector<cv::Mat> CameraParameters::GetRotationVectors() const { return rotation_vectors_; }
std::vector<cv::Mat> CameraParameters::GetTranslationVectors() const { return translation_vectors_; }

void CameraParameters::SetCameraMatrix(const cv::Mat& camera_matrix) { camera_matrix_ = camera_matrix; };
void CameraParameters::SetDistrotionCoefficients(const cv::Mat& distortion_coefficients) { distortion_coefficients_ = distortion_coefficients; };
void CameraParameters::SetRotationVectors(const std::vector<cv::Mat>& rotation_vectors) { rotation_vectors_ = rotation_vectors; };
void CameraParameters::SetTranslationVectors(const std::vector<cv::Mat>& translation_vectors) { translation_vectors_ = translation_vectors; };

bool CameraParameters::LoadFromFile(const std::string& filename)
{
	std::ifstream fin(filename);

	if (fin.is_open()) {
		double buffer { 0.0f };
		uint16_t rows;
		uint16_t columns;

		fin >> rows;
		fin >> columns;
		camera_matrix_ = cv::Mat(cv::Size(columns, rows), CV_64F);
		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < columns; ++col) {
				buffer = 0.0f;
				fin >> buffer;
				camera_matrix_.at<double>(row, col) = buffer;
			}
		}

		fin >> rows;
		fin >> columns;
		distortion_coefficients_ = cv::Mat::zeros(rows, columns, CV_64F);
		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < columns; ++col) {
				buffer = 0.0f;
				fin >> buffer;
				distortion_coefficients_.at<double>(row, col) = buffer;
			}
		}

		fin.close();
		return true;
	}

	return false;
}

bool CameraParameters::SaveToFile(const std::string& filename) const
{
	std::ofstream fout(filename);
	if (fout.is_open()) {
		double buffer { 0.0f };
		uint16_t rows;
		uint16_t columns;
		
		rows = camera_matrix_.rows;
		columns = camera_matrix_.cols;
		fout << rows << std::endl;
		fout << columns << std::endl;
		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < columns; ++col) {
				buffer = camera_matrix_.at<double>(row, col);
				fout << buffer << std::endl;
			}
		}

		rows = distortion_coefficients_.rows;
		columns = distortion_coefficients_.cols;
		fout << rows << std::endl;
		fout << columns << std::endl;
		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < columns; ++col) {
				buffer = distortion_coefficients_.at<double>(row, col);
				fout << buffer << std::endl;
			}
		}

		fout.close();
		return true;
	}
	
	return false;
}


void CameraCalibrationSettingsHandler::CreateSettingsFile()
{
	CameraCalibrationSettings settings;
    std::string calibration_setting_file_path;
    
    std::cout << " - Creating calibration settings file. Enter following fields." << std::endl << std::endl;

    std::cout << " Calibration grid pattern (available: \"chessboard\"): ";
    std::cin >> settings.calibration_grid_pattern_;
    if (settings.calibration_grid_pattern_ != "chessboard") {
        throw camera_calibration::CameraCalibrationExeption("unsupported board type");
    }

    std::cout << " Calibration board height: ";
    std::cin >> settings.calibration_board_size_.height;

    std::cout << " Calibration board width: ";
    std::cin >> settings.calibration_board_size_.width;

    std::cout << " Distance between points (centimeters): ";
    std::cin >> settings.distance_between_points_;

	std::cout << " Image source type (avilable: \"directory\", \"stream\"): ";
    std::cin >> settings.image_source_type_;
	if (settings.image_source_type_ != "directory" &&
		settings.image_source_type_ != "stream") {
        throw camera_calibration::CameraCalibrationExeption("unsupported image source type");
    }

    std::cout << " Image source path: ";
        std::cin >> settings.image_source_path_;

    std::cout << " Camera parameters file path: ";
    std::cin >> settings.camera_parameters_file_path_;

    std::cout << " Calibration settings file path: ";
        std::cin >> calibration_setting_file_path;

    std::cout << std::endl;

    nlohmann::json calibration_settings = {
        { "calibration_grid_pattern", settings.calibration_grid_pattern_ },
        { "calibration_board_size", { settings.calibration_board_size_.height, settings.calibration_board_size_.width } },
        { "distance_between_points", settings.distance_between_points_ },
		{ "image_source_type", settings.image_source_type_ },
        { "image_source_path", settings.image_source_path_ },
        { "camera_parameters_file_path", settings.camera_parameters_file_path_ }
    };

    std::ofstream fout(calibration_setting_file_path);
    fout << std::setw(4) << calibration_settings << std::endl;
    fout.close();
}

CameraCalibrationSettings 
	CameraCalibrationSettingsHandler::GetSettingsFromJson(const nlohmann::json& camera_calibration_settings)
{
	CameraCalibrationSettings settings;
	
	try {
		settings.calibration_grid_pattern_ = camera_calibration_settings["calibration_grid_pattern"].get<std::string>();
		if (settings.calibration_grid_pattern_ != "chessboard") {
			throw CameraCalibrationExeption("unsupported calibration grid pattern");
		}
		settings.calibration_board_size_.height = camera_calibration_settings["calibration_board_size"][0].get<int>();
		settings.calibration_board_size_.width = camera_calibration_settings["calibration_board_size"][1].get<int>();
		settings.distance_between_points_ = camera_calibration_settings["distance_between_points"].get<double>();
		settings.image_source_type_ = camera_calibration_settings["image_source_type"].get<std::string>();
		if (settings.image_source_type_ != "directory" && settings.image_source_type_ != "stream") {
			throw CameraCalibrationExeption("unsupported image source type");
		}
		settings.image_source_path_ = camera_calibration_settings["image_source_path"].get<std::string>();
		settings.camera_parameters_file_path_ = camera_calibration_settings["camera_parameters_file_path"].get<std::string>();
	}
	catch (nlohmann::json::parse_error excpt) {
		throw CameraCalibrationExeption("failed to parse settings");
	}

	return settings;
}


CameraCalibrationSettings::CameraCalibrationSettings()
{
	accuracy_criteria_ = cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.001);
    search_windows_size_ = cv::Size(11, 11);
    zero_zone_size_ = cv::Size(11, 11);
}

CameraCalibrationSettings& CameraCalibrationSettings::operator=(const CameraCalibrationSettings& calibration_settings)
{
	if (this == &calibration_settings) {
		return *this;
	}

	calibration_grid_pattern_ = calibration_settings.calibration_grid_pattern_;
    calibration_board_size_ = calibration_settings.calibration_board_size_;
    distance_between_points_= calibration_settings.distance_between_points_;
	image_source_type_= calibration_settings.image_source_type_;
    image_source_path_= calibration_settings.image_source_path_;
    camera_parameters_file_path_= calibration_settings.camera_parameters_file_path_;

    accuracy_criteria_= calibration_settings.accuracy_criteria_;
    search_windows_size_= calibration_settings.search_windows_size_;
    zero_zone_size_= calibration_settings.zero_zone_size_;

	return *this;
}

std::string CameraCalibrationSettings::GetCalibrationGridPattern() const { return calibration_grid_pattern_; }
cv::Size CameraCalibrationSettings::GetCalibrationBoardSize() const { return calibration_board_size_; }
double CameraCalibrationSettings::GetDistanceBetweenPoints() const { return distance_between_points_; }
std::string CameraCalibrationSettings::GetImageSourceType() const { return image_source_type_; }
std::string CameraCalibrationSettings::GetImageSourcePath() const { return image_source_path_; }
std::string CameraCalibrationSettings::GetCameraParametersFilePath() const { return camera_parameters_file_path_; }

void CameraCalibrationSettings::SetCalibrationGridPattern(const std::string& calibration_grid_pattern) {
	if (calibration_grid_pattern != "chessboard") {
		throw CameraCalibrationExeption("unsupported calibration grid pattern");
	} 
	calibration_grid_pattern_ = calibration_grid_pattern; 
}
void CameraCalibrationSettings::SetCalibrationBoardSize(const cv::Size& calibration_board_size) { 
	calibration_board_size_ = calibration_board_size; 
}
void CameraCalibrationSettings::SetDistanceBetweenPoints(const double& distance_between_points) { 
	distance_between_points_ = distance_between_points; 
}
void CameraCalibrationSettings::SetImageSourceType(const std::string& image_source_type) {
	if (image_source_type != "directory" && image_source_type != "stream") {
			throw CameraCalibrationExeption("unsupported image source type");
	}
	image_source_type_ = image_source_type; 
}
void CameraCalibrationSettings::SetImageSourcePath(const std::string& image_source_path) { 
	image_source_path_ = image_source_path; 
}
void CameraCalibrationSettings::SetCameraParametersFilePath(const std::string& camera_parameters_file_path) { 
	camera_parameters_file_path_ = camera_parameters_file_path; 
}


CameraCalibration::CameraCalibration(
	const CameraCalibrationSettings& calibration_settings,
    const std::vector<cv::Mat>& calibration_images_bgr) 
{
	calibration_images_.resize(calibration_images_bgr.size());
	for (int i { 0 }; i < calibration_images_bgr.size(); ++i) {
		cv::cvtColor(calibration_images_bgr[i], calibration_images_[i], cv::COLOR_BGR2GRAY);
	}
	calibration_settings_ = calibration_settings;

	CalculateReferenceGridPoints();
	CalculateRealChessboardPoints();
	
	reference_points_.resize(real_points_.size(), reference_points_[0]);
	camera_parameters_.distortion_coefficients_ = cv::Mat::zeros(8, 1, CV_64F);

	cv::calibrateCamera(
		reference_points_, 
		real_points_, 
		calibration_settings_.calibration_board_size_, 
		camera_parameters_.camera_matrix_, 
		camera_parameters_.distortion_coefficients_, 
		camera_parameters_.rotation_vectors_, 
		camera_parameters_.translation_vectors_);
}

void CameraCalibration::CalculateReferenceGridPoints()
{
	for (int i { 0 }; i < calibration_settings_.calibration_board_size_.height; ++i) {
		for (int j = 0; j < calibration_settings_.calibration_board_size_.width; ++j) {
			reference_points_[0].push_back(cv::Point3f(
				j * calibration_settings_.distance_between_points_, 
				i * calibration_settings_.distance_between_points_, 
				0.0f));
		}
	}
}

void CameraCalibration::CalculateRealChessboardPoints()
{
	for (auto image_iterator = calibration_images_.begin(); image_iterator != calibration_images_.end(); ++image_iterator) {
		std::vector<cv::Point2f> corners_buffer;
		if (cv::findChessboardCorners(
			*image_iterator, 
			calibration_settings_.calibration_board_size_, 
			corners_buffer, 
			cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE)) 
		{
			cv::cornerSubPix(
				*image_iterator,
				corners_buffer,
				calibration_settings_.search_windows_size_,
				calibration_settings_.zero_zone_size_,
				calibration_settings_.accuracy_criteria_);
			real_points_.push_back(corners_buffer);
		}
	}
}


void UndistortPoint (
    const cv::Point2f& src, 
    cv::Point2f& dst, 
    const CameraParameters& camera_parameters,
    const cv::Size &image_size = cv::Size(-1, -1)) 
{
	std::vector<cv::Point2f> src_temp { src };
	std::vector<cv::Point2f> dst_temp { dst };

	if (image_size == cv::Size(-1, -1)) {	
		cv::undistortPoints(src_temp, dst_temp, camera_parameters.GetCameraMatrix(), camera_parameters.GetDistrotionCoefficients());
	} 
	else {
		cv::Mat optimal_camera_matrix_ =
			cv::getOptimalNewCameraMatrix(camera_parameters.GetCameraMatrix(), camera_parameters.GetDistrotionCoefficients(), image_size, 1.0);
		cv::undistortPoints(src_temp, dst_temp, optimal_camera_matrix_, camera_parameters.GetDistrotionCoefficients());
	}

	dst = dst_temp[0];
}


} // namespace camera_calibration