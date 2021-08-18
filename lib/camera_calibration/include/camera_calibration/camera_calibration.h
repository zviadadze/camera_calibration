#ifndef CAMERA_CALIBRATION_H_
#define CAMERA_CALIBRATION_H_


#include<opencv2/calib3d.hpp>


class CameraCalibration {

public:

    CameraCalibration(
        const std::vector<cv::Mat>& calibration_images,
        const cv::Size& chessboard_dimensions,
        const double& calibration_square_length
    );
    CameraCalibration(const CameraCalibration &) = delete;
    CameraCalibration(CameraCalibration &&) = delete;
    CameraCalibration &operator=(const CameraCalibration &) = delete;
    CameraCalibration &operator=(CameraCalibration &&) = delete;
    ~CameraCalibration() {};

    bool SaveCalibrationParameters(const std::string& filename);
    bool LoadCalibrationParameters(const std::string& filename);

private:

    void CreateReferenceChessboardCorners();
    void GetRealChessboardCorners();

private:

    std::vector<cv::Mat> calibration_images;
    cv::Size chessboard_dimensions;
    float calibration_square_length;

    std::vector<std::vector<cv::Point3f>> reference_corner_points{1};
    std::vector<std::vector<cv::Point2f>> chessboard_corner_points;
    std::vector<cv::Mat> rotation_vectors, translation_vectors;

    cv::Mat camera_matrix;
    cv::Mat distortion_coefficients;

public:

    friend bool cv::findChessboardCorners(
        cv::InputArray image, 
        cv::Size patternSize, 
        cv::OutputArray corners, 
        int flags
    );
    friend double cv::calibrateCamera(
        cv::InputArrayOfArrays objectPoints, 
        cv::InputArrayOfArrays imagePoints,
        cv::Size imageSize,
        cv::InputOutputArray cameraMatrix,
        cv::InputOutputArray distCoeffs,
        cv::OutputArrayOfArrays rvecs,
        cv::OutputArrayOfArrays tvecs,
        int flags,
        cv::TermCriteria criteria
    );

};


#endif
