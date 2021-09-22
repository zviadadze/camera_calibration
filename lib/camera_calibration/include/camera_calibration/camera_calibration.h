#ifndef CAMERA_CALIBRATION_H_
#define CAMERA_CALIBRATION_H_

#include<opencv2/calib3d.hpp>


class CameraCalibration {

public:

    enum CalibrationGridPattern {
            CHESSBOARD,
            SYMMETRIC_CIRCLE_GRID,
            ASYMMETRIC_CIRCLE_GRID
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
        const std::vector<cv::Mat> &calibration_images,
        const cv::Size &calibration_board_size,
        const double &distance_between_real_points,
        const CalibrationGridPattern &calibration_grid_pattern,
        const cv::Ptr<cv::SimpleBlobDetector> &circle_detector =
            cv::SimpleBlobDetector::create(),
        const cv::TermCriteria &accuracy_criteria =
		    cv::TermCriteria(
                cv::TermCriteria::EPS + cv::TermCriteria::COUNT,
                CameraCalibration::MAX_IREATIONS_COUNT_, 
                CameraCalibration::EPSILON_)
    );
    CameraCalibration(const CameraCalibration &) = delete;
    CameraCalibration(CameraCalibration &&) = delete;
    CameraCalibration &operator=(const CameraCalibration &) = delete;
    CameraCalibration &operator=(CameraCalibration &&) = delete;
    ~CameraCalibration() {};


    cv::Mat GetCameraMatrix() { return camera_matrix_; }
    cv::Mat GetDistortionCoefficients() { return distortion_coefficients_; }
    void SetCameraMatrix(const cv::Mat &camera_matrix) { camera_matrix_ = camera_matrix; }
    void SetDistortionCoefficients(const cv::Mat &distortion_coefficients) {
        distortion_coefficients_ = distortion_coefficients;
    }

    bool SaveCalibrationParameters(const std::string &filename);
    bool LoadCalibrationParameters(const std::string &filename);    

private:

    void CreateReferenceGridPoints();
    void GetRealChessboardPoints();
    void GetRealCirclesGridPoints();

public:

    static constexpr int MAX_IREATIONS_COUNT_ { 30 };
    static constexpr double EPSILON_ { 0.0001 };

private:

    cv::Mat camera_matrix_;
    cv::Mat distortion_coefficients_;
    std::vector<cv::Mat> rotation_vectors_;
    std::vector<cv::Mat> translation_vectors_;
    
    std::vector<cv::Mat> calibration_images_;
    cv::Size calibration_board_size_;
    double distance_between_real_points_;

    std::vector<std::vector<cv::Point3f>> reference_points_{1};
    std::vector<std::vector<cv::Point2f>> real_points_;
    
    cv::Ptr<cv::SimpleBlobDetector> circle_detector_;
    CalibrationGridPattern calibration_grid_pattern_;

    cv::TermCriteria accuracy_criteria_;

    const cv::Size SEARCH_WINDOW_SIZE_ {11, 11};
    const cv::Size ZERO_ZONE_SIZE_ {11, 11};
    
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

void UndistortPoint (
    const cv::Point2f &src, 
    cv::Point2f &dst, 
    const cv::Mat &camera_matrix,
    const cv::Mat  distortion_coefficients, 
    const cv::Size &image_size
);


#endif
