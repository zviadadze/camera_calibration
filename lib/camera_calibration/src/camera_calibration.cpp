#include "camera_calibration/camera_calibration.h"


CameraCalibration::CameraCalibration(
	const std::vector<cv::Mat>& calibration_images,
	const cv::Size& chessboard_dimensions,
	const double& square_edge_length
) {
	this->calibration_images = calibration_images;
	this->chessboard_dimensions = chessboard_dimensions;
	this->square_edge_length = square_edge_length;

	CreateReferenceChessboardCorners();
	GetRealChessboardCorners();
	
	reference_corner_points.resize(chessboard_corner_points.size(), reference_corner_points[0]);
	distortion_coefficients = cv::Mat::zeros(8, 1, CV_64F);

	cv::calibrateCamera(
		reference_corner_points, 
		chessboard_corner_points, 
		chessboard_dimensions, 
		camera_matrix, 
		distortion_coefficients, 
		rotation_vectors, 
		translation_vectors
	);
}

void CameraCalibration::CreateReferenceChessboardCorners() {
	for (int i = 0; i < chessboard_dimensions.height; ++i) {
		for (int j = 0; j < chessboard_dimensions.width; ++j) {
			reference_corner_points[0].push_back(cv::Point3f(j * square_edge_length, i * square_edge_length, 0.0f));
		}
	}
}

void CameraCalibration::GetRealChessboardCorners() {
	for (std::vector<cv::Mat>::iterator iter = calibration_images.begin(); iter != calibration_images.end(); ++iter) {
		std::vector<cv::Point2f> corners_buffer;
		if (cv::findChessboardCorners(
			*iter, 
			chessboard_dimensions, 
			corners_buffer, 
			cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE)
		) {
			chessboard_corner_points.push_back(corners_buffer);
		}
	}
}

bool CameraCalibration::SaveCameraCalibrationParameters(const std::string& filename) {
	std::ofstream fout(filename);
	if (fout.is_open()) {
		uint16_t rows;
		uint16_t columns;
		
		rows = camera_matrix.rows;
		columns = camera_matrix.cols;
		
		fout << rows << std::endl;
		fout << columns << std::endl;

		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < columns; col++) {
				double value = camera_matrix.at<double>(row, col);
				fout << value << std::endl;
			}
		}

		rows = distortion_coefficients.rows;
		columns = distortion_coefficients.cols;

		fout << rows << std::endl;
		fout << columns << std::endl;
		
		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < columns; col++) {
				double value = distortion_coefficients.at<double>(row, col);
				fout << value << std::endl;
			}
		}

		fout.close();
		return true;
	}
	
	return false;
}

bool CameraCalibration::LoadCameraCalibrationParameters(const std::string& filename) {
	std::ifstream fin(filename);
	
	std::cout << "Camera calibration parameters:" << std::endl;

	if (fin.is_open()) {
		uint16_t rows;
		uint16_t columns;

		fin >> rows;
		fin >> columns;

		camera_matrix = cv::Mat(cv::Size(columns, rows), CV_64F);

		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < columns; col++) {
				double read = 0.0f;
				fin >> read;
				camera_matrix.at<double>(row, col) = read;
				std::cout << camera_matrix.at<double>(row, col) << std::endl;
			}
		}

		fin >> rows;
		fin >> columns;

		distortion_coefficients = cv::Mat::zeros(rows, columns, CV_64F);

		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < columns; col++) {
				double read = 0.0f;
				fin >> read;
				distortion_coefficients.at<double>(row, col) = read;
				std::cout << distortion_coefficients.at<double>(row, col) << std::endl;
			}
		}

		fin.close();
		return true;
	}

	return 0;
}