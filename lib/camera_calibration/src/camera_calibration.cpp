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
	const cv::Size& chessboard_dimensions,
	const double& calibration_square_length
) {
	this->calibration_images_ = calibration_images;
	this->chessboard_dimensions_ = chessboard_dimensions;
	this->calibration_square_length_ = calibration_square_length;

	CreateReferenceChessboardCorners();
	GetRealChessboardCorners();
	
	reference_corner_points_.resize(chessboard_corner_points_.size(), reference_corner_points_[0]);
	distortion_coefficients_ = cv::Mat::zeros(8, 1, CV_64F);

	cv::calibrateCamera(
		reference_corner_points_, 
		chessboard_corner_points_, 
		chessboard_dimensions_, 
		camera_matrix_, 
		distortion_coefficients_, 
		rotation_vectors_, 
		translation_vectors_
	);
}

void CameraCalibration::CreateReferenceChessboardCorners() {
	for (int i = 0; i < chessboard_dimensions_.height; ++i) {
		for (int j = 0; j < chessboard_dimensions_.width; ++j) {
			reference_corner_points_[0].push_back(cv::Point3f(j * calibration_square_length_, i * calibration_square_length_, 0.0f));
		}
	}
}

void CameraCalibration::GetRealChessboardCorners() {
	for (std::vector<cv::Mat>::iterator iter = calibration_images_.begin(); iter != calibration_images_.end(); ++iter) {
		std::vector<cv::Point2f> corners_buffer;
		if (cv::findChessboardCorners(
			*iter, 
			chessboard_dimensions_, 
			corners_buffer, 
			cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE)
		) {
			chessboard_corner_points_.push_back(corners_buffer);
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

void CameraCalibration::UndistortPoint (cv::Point2f src, cv::Point2f dst, cv::Size image_size) {
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