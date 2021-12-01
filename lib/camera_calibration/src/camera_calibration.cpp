#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "camera_calibration/camera_calibration.h"

namespace camera_calibration {


CameraCalibrationParameters::CameraCalibrationParameters()
{
	max_iterations_count = 30;
    epsilon = 0.0001;
    search_windows_size = cv::Size(11, 11);
    zero_zone_size = cv::Size(11, 11);
}

CameraCalibrationParameters::CameraCalibrationParameters(const nlohmann::json& camera_calibration_parameters_json)
{
	// Написать реализацию
}


CameraCalibration::CameraCalibration(
	const CameraCalibrationParameters& calibration_parameters,
    const std::vector<cv::Mat>& calibration_images) 
{
	calibration_images_.resize(calibration_images.size());
	for (int i = 0; i < calibration_images_.size(); ++i) {
		cv::cvtColor(calibration_images[i], calibration_images_[i], cv::COLOR_BGR2GRAY);
	}
	calibration_parameters_ = calibration_parameters;

	CreateReferenceGridPoints();
	if (calibration_parameters_.calibration_grid_pattern == CalibrationGridPatterns::CHESSBOARD) {
		GetRealChessboardPoints();
	}
	else {
		GetRealCirclesGridPoints();
	}
	
	reference_points_.resize(real_points_.size(), reference_points_[0]);
	camera_parameters_.distortion_coefficients = cv::Mat::zeros(8, 1, CV_64F);

	cv::calibrateCamera(
		reference_points_, 
		real_points_, 
		calibration_parameters_.calibration_board_size, 
		camera_parameters_.camera_matrix, 
		camera_parameters_.distortion_coefficients, 
		camera_parameters_.rotation_vectors, 
		camera_parameters_.translation_vectors);
}

void CameraCalibration::CreateReferenceGridPoints()
{
	for (int i = 0; i < calibration_parameters_.calibration_board_size.height; ++i) {
		for (int j = 0; j < calibration_parameters_.calibration_board_size.width; ++j) {
			reference_points_[0].push_back(cv::Point3f(
				j * calibration_parameters_.distance_between_real_points, 
				i * calibration_parameters_.distance_between_real_points, 0.0f));
		}
	}
}

void CameraCalibration::GetRealChessboardPoints()
{
	for (std::vector<cv::Mat>::iterator iter = calibration_images_.begin(); iter != calibration_images_.end(); ++iter) {
		std::vector<cv::Point2f> corners_buffer;
		if (cv::findChessboardCorners(
			*iter, 
			calibration_parameters_.calibration_board_size, 
			corners_buffer, 
			cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE)) 
		{
			cv::cornerSubPix(
				*iter,
				corners_buffer,
				calibration_parameters_.search_windows_size,
				calibration_parameters_.zero_zone_size,
				calibration_parameters_.accuracy_criteria);
			real_points_.push_back(corners_buffer);
		}
	}
}

void CameraCalibration::GetRealCirclesGridPoints()
{
	for (std::vector<cv::Mat>::iterator iter = calibration_images_.begin(); iter != calibration_images_.end(); ++iter) {
		std::vector<cv::Point2f> centers_buffer;
		bool pattern_found = false;

		if (calibration_parameters_.calibration_grid_pattern == CalibrationGridPatterns::SYMMETRIC_CIRCLE_GRID) {
			pattern_found = cv::findCirclesGrid(
				*iter, 
				calibration_parameters_.calibration_board_size, 
				centers_buffer, 
				cv::CALIB_CB_SYMMETRIC_GRID,
				calibration_parameters_.circle_detector);
		} 
		else if (calibration_parameters_.calibration_grid_pattern == CalibrationGridPatterns::ASYMMETRIC_CIRCLE_GRID) {
			pattern_found = cv::findCirclesGrid(
				*iter, 
				calibration_parameters_.calibration_board_size, 
				centers_buffer, 
				cv::CALIB_CB_ASYMMETRIC_GRID,
				calibration_parameters_.circle_detector);
		}

		if (pattern_found) {
			cv::cornerSubPix(
				*iter, 
				centers_buffer,
				calibration_parameters_.search_windows_size, 
				calibration_parameters_.zero_zone_size, 
				calibration_parameters_.accuracy_criteria);
			real_points_.push_back(centers_buffer);
		}
	}
}


bool CameraParameters::LoadFromFile(const std::string& filename)
{
	std::ifstream fin(filename);

	if (fin.is_open()) {
		double buffer { 0.0f };
		uint16_t rows;
		uint16_t columns;

		fin >> rows;
		fin >> columns;
		camera_matrix = cv::Mat(cv::Size(columns, rows), CV_64F);
		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < columns; ++col) {
				buffer = 0.0f;
				fin >> buffer;
				camera_matrix.at<double>(row, col) = buffer;
			}
		}

		fin >> rows;
		fin >> columns;
		distortion_coefficients = cv::Mat::zeros(rows, columns, CV_64F);
		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < columns; ++col) {
				buffer = 0.0f;
				fin >> buffer;
				distortion_coefficients.at<double>(row, col) = buffer;
			}
		}

		fin.close();
		return true;
	}

	return false;
}

bool CameraParameters::SaveToFile(const std::string& filename) 
{
	std::ofstream fout(filename);
	if (fout.is_open()) {
		double buffer { 0.0f };
		uint16_t rows;
		uint16_t columns;
		
		rows = camera_matrix.rows;
		columns = camera_matrix.cols;
		fout << rows << std::endl;
		fout << columns << std::endl;
		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < columns; ++col) {
				buffer = camera_matrix.at<double>(row, col);
				fout << buffer << std::endl;
			}
		}

		rows = distortion_coefficients.rows;
		columns = distortion_coefficients.cols;
		fout << rows << std::endl;
		fout << columns << std::endl;
		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < columns; ++col) {
				buffer = distortion_coefficients.at<double>(row, col);
				fout << buffer << std::endl;
			}
		}

		fout.close();
		return true;
	}
	
	return false;
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
		cv::undistortPoints(src_temp, dst_temp, camera_parameters.camera_matrix, camera_parameters.distortion_coefficients);
	} 
	else {
		cv::Mat optimal_camera_matrix =
			cv::getOptimalNewCameraMatrix(camera_parameters.camera_matrix, camera_parameters.distortion_coefficients, image_size, 1.0);
		cv::undistortPoints(src_temp, dst_temp, optimal_camera_matrix, camera_parameters.distortion_coefficients);
	}

	dst = dst_temp[0];
}


} // camera_calibration