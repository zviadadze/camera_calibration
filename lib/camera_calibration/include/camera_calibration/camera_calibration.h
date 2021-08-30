#ifndef CAMERA_CALIBRATION_H_
#define CAMERA_CALIBRATION_H_

#include<opencv2/calib3d.hpp>


class CameraCalibration {

public:

    CameraCalibration (const std::string filename) {
        LoadCalibrationParameters(filename);
    }
    CameraCalibration(cv::Mat camera_matrix, cv::Mat distortion_coefficients) {
        this->camera_matrix_ = camera_matrix;
        this->distortion_coefficients_ = distortion_coefficients;
    }
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

    cv::Mat GetCameraMatrix() { return camera_matrix_; }
    cv::Mat GetDistortionCoefficients() { return distortion_coefficients_; }

    bool SaveCalibrationParameters(const std::string& filename);
    void UndistortPoint (const cv::Point2f& src, cv::Point2f& dst, const cv::Size& image_size);

private:

    bool LoadCalibrationParameters(const std::string& filename);
    void CreateReferenceChessboardCorners();
    void GetRealChessboardCorners();

private:

    cv::Mat camera_matrix_;
    cv::Mat distortion_coefficients_;

    std::vector<cv::Mat> calibration_images_;
    cv::Size chessboard_dimensions_;
    float calibration_square_length_;

    std::vector<std::vector<cv::Point3f>> reference_corner_points_{1};
    std::vector<std::vector<cv::Point2f>> chessboard_corner_points_;
    std::vector<cv::Mat> rotation_vectors_, translation_vectors_;

    cv::TermCriteria criteria_ { CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.001 };

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
