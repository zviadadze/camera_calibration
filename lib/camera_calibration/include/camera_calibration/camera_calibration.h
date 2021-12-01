#ifndef CAMERA_CALIBRATION_H_
#define CAMERA_CALIBRATION_H_

#include <opencv2/calib3d.hpp>

#include "nlohmann/json.hpp"

namespace camera_calibration {


struct CameraParameters
{
    cv::Mat camera_matrix;
    cv::Mat distortion_coefficients;
    std::vector<cv::Mat> rotation_vectors;
    std::vector<cv::Mat> translation_vectors;


    bool SaveToFile(const std::string& filename);
    bool LoadFromFile(const std::string& filename); 
};


enum CalibrationGridPatterns
{
    CHESSBOARD,
    SYMMETRIC_CIRCLE_GRID,
    ASYMMETRIC_CIRCLE_GRID
};


struct CameraCalibrationParameters
{
    CalibrationGridPatterns calibration_grid_pattern;
    cv::Size calibration_board_size;
    double distance_between_real_points;

    cv::Ptr<cv::SimpleBlobDetector> circle_detector;
    
    cv::TermCriteria accuracy_criteria;
    int max_iterations_count;
    double epsilon;

    cv::Size search_windows_size;
    cv::Size zero_zone_size;

    CameraCalibrationParameters();
    CameraCalibrationParameters(const nlohmann::json& camera_calibration_parameters_json);

    CameraCalibrationParameters() = delete;
    CameraCalibrationParameters(const CameraCalibration&) = delete;
    CameraCalibrationParameters(CameraCalibration&&) = delete;
    CameraCalibrationParameters& operator=(const CameraCalibration&) = delete;
    CameraCalibrationParameters& operator=(CameraCalibration&&) = delete;

    ~CameraCalibrationParameters() {};
};


class CameraCalibration 
{

public:

    CameraCalibration(
        const CameraCalibrationParameters& camera_calibration_parameters,
        const std::vector<cv::Mat>& calibration_images);

    CameraCalibration() = delete;
    CameraCalibration(const CameraCalibration&) = delete;
    CameraCalibration(CameraCalibration&&) = delete;
    CameraCalibration& operator=(const CameraCalibration&) = delete;
    CameraCalibration& operator=(CameraCalibration&&) = delete;

    ~CameraCalibration() {};

    CameraParameters ExtractCameraParameters() const { return camera_parameters_; }


private:

    CameraCalibrationParameters calibration_parameters_;
    std::vector<cv::Mat> calibration_images_;
    CameraParameters camera_parameters_;

    std::vector<std::vector<cv::Point3f>> reference_points_ { 1 };
    std::vector<std::vector<cv::Point2f>> real_points_;


    void CreateReferenceGridPoints();
    void GetRealChessboardPoints();
    void GetRealCirclesGridPoints();


    friend bool cv::findChessboardCorners(
        cv::InputArray image, 
        cv::Size patternSize, 
        cv::OutputArray corners, 
        int flags);

    friend bool findCirclesGrid(
        cv::InputArray image, 
        cv::Size patternSize,
        cv::OutputArray centers, 
        int flags,
        const cv::Ptr<cv::FeatureDetector>& blobDetector);

    friend double cv::calibrateCamera(
        cv::InputArrayOfArrays objectPoints, 
        cv::InputArrayOfArrays imagePoints,
        cv::Size imageSize,
        cv::InputOutputArray cameraMatrix,
        cv::InputOutputArray distCoeffs,
        cv::OutputArrayOfArrays rvecs,
        cv::OutputArrayOfArrays tvecs,
        int flags,
        cv::TermCriteria criteria);

};


void UndistortPoint(
    const cv::Point2f& src, 
    cv::Point2f& dst, 
    const CameraParameters& camera_parameters,
    const cv::Size& image_size);


} // camera_calibration

#endif
