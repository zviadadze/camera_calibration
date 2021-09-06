#ifndef CAMERA_CALIBRATION_H_
#define CAMERA_CALIBRATION_H_

#include<opencv2/calib3d.hpp>


class CameraCalibration {

public:

    enum CirclePatternType {
            SYMMETRIC,
            ASYMMETRIC
    };

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
        const cv::Size& calibration_board_size,
        const double& distance_between_real_points,
        const cv::TermCriteria& accuracy_criteria =
		    cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.0001)
    );
    CameraCalibration(
        const std::vector<cv::Mat>& calibration_images,
        const cv::Size& calibration_board_size,
        const double& distance_between_real_points,
        const cv::Ptr<cv::SimpleBlobDetector>& circle_detector, 
        const CirclePatternType& circle_pattern_type,
        const cv::TermCriteria& accuracy_criteria =
		    cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.0001)
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
    void CreateReferenceGridPoints();
    void GetRealChessboardPoints();
    void GetRealCirclesGridPoints();

private:

    cv::Mat camera_matrix_;
    cv::Mat distortion_coefficients_;
    std::vector<cv::Mat> rotation_vectors_;
    std::vector<cv::Mat> translation_vectors_;
    
    std::vector<cv::Mat> calibration_images_;
    cv::Size calibration_board_size_;
    float distance_between_real_points_;

    std::vector<std::vector<cv::Point3f>> reference_points_{1};
    std::vector<std::vector<cv::Point2f>> real_points_;
    
    cv::Ptr<cv::SimpleBlobDetector> circle_detector_;
    CirclePatternType circle_pattern_type_;

    cv::TermCriteria accuracy_criteria_;
    
private:

    friend bool cv::findChessboardCorners(
        cv::InputArray image, 
        cv::Size patternSize, 
        cv::OutputArray corners, 
        int flags
    );
    friend bool findCirclesGrid(
        cv::InputArray image, 
        cv::Size patternSize,
        cv::OutputArray centers, 
        int flags = cv::CALIB_CB_SYMMETRIC_GRID,
        const cv::Ptr<cv::FeatureDetector> &blobDetector = 
            cv::SimpleBlobDetector::create()
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
