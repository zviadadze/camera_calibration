#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include<opencv2/core.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>

#include "camera_calibration/camera_calibration.h"


CameraCalibration::CameraCalibration(
	const std::vector<cv::Mat>& calibration_images,
	const cv::Size& calibration_board_size,
	const double& distance_between_real_points,
	const cv::TermCriteria& accuracy_criteria
) {
	calibration_images_.resize(calibration_images.size());
	for(int i = 0; i < calibration_images_.size(); ++i) {
		cv::cvtColor(calibration_images[i], calibration_images_[i], cv::COLOR_BGR2GRAY);
	}
	calibration_board_size_ = calibration_board_size;
	distance_between_real_points_ = distance_between_real_points;
	accuracy_criteria_ = accuracy_criteria;

	CreateReferenceGridPoints();
	GetRealChessboardPoints();
	
	reference_points_.resize(real_points_.size(), reference_points_[0]);
	distortion_coefficients_ = cv::Mat::zeros(8, 1, CV_64F);

	cv::calibrateCamera(
		reference_points_, 
		real_points_, 
		calibration_board_size_, 
		camera_matrix_, 
		distortion_coefficients_, 
		rotation_vectors_, 
		translation_vectors_
	);
}

CameraCalibration::CameraCalibration(
	const std::vector<cv::Mat>& calibration_images,
	const cv::Size& calibration_board_size,
	const double& distance_between_real_points,
	const cv::Ptr<cv::SimpleBlobDetector>& circle_detector,
	const CirclePatternType& circle_pattern_type,
	const cv::TermCriteria& accuracy_criteria
) {
	calibration_images_.resize(calibration_images.size());
	for(int i = 0; i < calibration_images_.size(); ++i) {
		cv::cvtColor(calibration_images[i], calibration_images_[i], cv::COLOR_BGR2GRAY);
	}
	calibration_board_size_ = calibration_board_size;
	distance_between_real_points_ = distance_between_real_points_;
	circle_detector_ = circle_detector;
	circle_pattern_type_ = circle_pattern_type;
	accuracy_criteria_ = accuracy_criteria;

	CreateReferenceGridPoints();
	GetRealCirclesGridPoints();
	
	reference_points_.resize(real_points_.size(), reference_points_[0]);
	distortion_coefficients_ = cv::Mat::zeros(8, 1, CV_64F);

	cv::calibrateCamera(
		reference_points_, 
		real_points_, 
		calibration_board_size_, 
		camera_matrix_, 
		distortion_coefficients_, 
		rotation_vectors_, 
		translation_vectors_
	);
}

void CameraCalibration::CreateReferenceGridPoints() {
	for (int i = 0; i < calibration_board_size_.height; ++i) {
		for (int j = 0; j < calibration_board_size_.width; ++j) {
			reference_points_[0].push_back(cv::Point3f(j * distance_between_real_points_, i * distance_between_real_points_, 0.0f));
		}
	}
}

void CameraCalibration::GetRealChessboardPoints() {
	for (std::vector<cv::Mat>::iterator iter = calibration_images_.begin(); iter != calibration_images_.end(); ++iter) {
		std::vector<cv::Point2f> corners_buffer;
		if (cv::findChessboardCorners(
			*iter, 
			calibration_board_size_, 
			corners_buffer, 
			cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE)
		) {
			cv::cornerSubPix(*iter, corners_buffer, cv::Size(11, 11), cv::Size(-1, -1), accuracy_criteria_);
			real_points_.push_back(corners_buffer);
		}
	}
}

void CameraCalibration::GetRealCirclesGridPoints() {
	for (std::vector<cv::Mat>::iterator iter = calibration_images_.begin(); iter != calibration_images_.end(); ++iter) {
		std::vector<cv::Point2f> centers_buffer;
		bool grid_found = false;

		if (circle_pattern_type_ == CameraCalibration::CirclePatternType::SYMMETRIC) {
			grid_found = cv::findCirclesGrid(
				*iter, 
				calibration_board_size_, 
				centers_buffer, 
				cv::CALIB_CB_SYMMETRIC_GRID,
				circle_detector_
			);
		} else if (circle_pattern_type_ == CameraCalibration::CirclePatternType::ASYMMETRIC) {
			grid_found = cv::findCirclesGrid(
				*iter, 
				calibration_board_size_, 
				centers_buffer, 
				cv::CALIB_CB_ASYMMETRIC_GRID,
				circle_detector_
			);
		}

		if (grid_found) {
			cv::cornerSubPix(*iter, centers_buffer, cv::Size(11, 11), cv::Size(-1, -1), accuracy_criteria_);
			real_points_.push_back(centers_buffer);
		}
	}
}

bool CameraCalibration::LoadCalibrationParameters(const std::string& filename) {
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

bool CameraCalibration::SaveCalibrationParameters(const std::string& filename) {
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

void CameraCalibration::UndistortPoint (const cv::Point2f& src, cv::Point2f& dst, const cv::Size& image_size) {
	std::vector<cv::Point2f> src_temp = { src };
	std::vector<cv::Point2f> dst_temp = { dst };

	if (image_size == cv::Size(-1, -1)) {	
		cv::undistortPoints(src_temp, dst_temp, camera_matrix_, distortion_coefficients_);
	} else {
		cv::Mat optimal_camera_matrix =
			cv::getOptimalNewCameraMatrix(camera_matrix_, distortion_coefficients_, image_size, 1.0);
		cv::undistortPoints(src_temp, dst_temp, optimal_camera_matrix, distortion_coefficients_);
	}

	dst = dst_temp[0];
}