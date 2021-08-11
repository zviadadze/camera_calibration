#ifndef CAMERA_CALIBRATION_H_
#define CAMERA_CALIBRATION_H_


#include<opencv2/core.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/calib3d.hpp>

#include <sstream>
#include <iostream>
#include <fstream>


class CameraCalibration {

public:

    CameraCalibration(
        const std::vector<cv::Mat>& calibration_images,
        const cv::Size& chessboard_dimensions,
        const double& square_edge_length
    );
    CameraCalibration(const CameraCalibration &) = delete;
    CameraCalibration(CameraCalibration &&) = delete;
    CameraCalibration &operator=(const CameraCalibration &) = delete;
    CameraCalibration &operator=(CameraCalibration &&) = delete;
    ~CameraCalibration() {};

    bool SaveCameraCalibrationParameters(const std::string& filename);
    bool LoadCameraCalibrationParameters(const std::string& filename);

private:

    void CreateReferenceChessboardCorners();
    void GetRealChessboardCorners();

private:

    cv::Size chessboard_dimensions;
    float square_edge_length;
    std::vector<cv::Mat> calibration_images;

    std::vector<std::vector<cv::Point3f>> reference_corner_points{1};
    std::vector<std::vector<cv::Point2f>> chessboard_corner_points;
    std::vector<cv::Mat> rotation_vectors, translation_vectors;

    cv::Mat camera_matrix;
    cv::Mat distortion_coefficients;

public:

    friend bool cv::findChessboardCorners(
        InputArray image, 
        Size patternSize, 
        OutputArray corners, 
        int flags
    );
    friend double cv::calibrateCamera(
        InputArrayOfArrays objectPoints, 
        InputArrayOfArrays imagePoints,
        Size imageSize,
        InputOutputArray cameraMatrix,
        InputOutputArray distCoeffs,
        OutputArrayOfArrays rvecs,
        OutputArrayOfArrays tvecs,
        int flags,
        TermCriteria criteria
    );

};

#endif
