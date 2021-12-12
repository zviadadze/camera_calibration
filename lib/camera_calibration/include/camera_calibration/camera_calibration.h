#ifndef CAMERA_CALIBRATION_H_
#define CAMERA_CALIBRATION_H_

#include <opencv2/calib3d.hpp>

#include "nlohmann/json.hpp"

namespace camera_calibration {


class CameraCalibrationExeption : public std::exception
{
public:

	CameraCalibrationExeption(std::string error) {
        error_message = " Error: " + error + '.';
    }

	const char* what() const noexcept { return error_message.c_str(); }

private:

	std::string error_message;
};


class CameraParameters
{
public:

    cv::Mat GetCameraMatrix() const;
    cv::Mat GetDistrotionCoefficients() const;
    std::vector<cv::Mat> GetRotationVectors() const;
    std::vector<cv::Mat> GetTranslationVectors() const;

    void SetCameraMatrix(const cv::Mat&);
    void SetDistrotionCoefficients(const cv::Mat&);
    void SetRotationVectors(const std::vector<cv::Mat>&);
    void SetTranslationVectors(const std::vector<cv::Mat>&);

    bool SaveToFile(const std::string& filename) const;
    bool LoadFromFile(const std::string& filename);

    friend class CameraCalibration;

private:

    cv::Mat camera_matrix_;
    cv::Mat distortion_coefficients_;
    std::vector<cv::Mat> rotation_vectors_;
    std::vector<cv::Mat> translation_vectors_;
};


class CameraCalibrationSettings;

class CameraCalibrationSettingsHandler
{
public:

    static void CreateSettingsFile();
    static CameraCalibrationSettings GetSettingsFromJson(const nlohmann::json&);
};


class CameraCalibrationSettings final
{
public:

    CameraCalibrationSettings();
    CameraCalibrationSettings& operator=(const CameraCalibrationSettings&);

    std::string GetCalibrationGridPattern() const;
    cv::Size GetCalibrationBoardSize() const;
    double GetDistanceBetweenPoints() const;
    std::string GetImageSourceType() const;
    std::string GetImageSourcePath() const;
    std::string GetCameraParametersFilePath() const;

    void SetCalibrationGridPattern(const std::string&);
    void SetCalibrationBoardSize(const cv::Size&);
    void SetDistanceBetweenPoints(const double&);
    void SetImageSourceType(const std::string&);
    void SetImageSourcePath(const std::string&);
    void SetCameraParametersFilePath(const std::string&);
    
    friend class CameraCalibrationSettingsHandler;
    friend class CameraCalibration;

private:

    std::string calibration_grid_pattern_;
    cv::Size calibration_board_size_;
    double distance_between_points_;
    std::string image_source_type_;
    std::string image_source_path_;
    std::string camera_parameters_file_path_;

    cv::TermCriteria accuracy_criteria_;
    cv::Size search_windows_size_;
    cv::Size zero_zone_size_;
};


class CameraCalibration final
{
public:
    
    CameraCalibration(
        const CameraCalibrationSettings& camera_calibration_settings,
        const std::vector<cv::Mat>& calibration_images_bgr);
        
    CameraCalibration() = delete;
    CameraCalibration(const CameraCalibration&) = delete;
    CameraCalibration(CameraCalibration&&) = delete;
    CameraCalibration& operator=(const CameraCalibration&) = delete;
    CameraCalibration& operator=(CameraCalibration&&) = delete;
    
    ~CameraCalibration() {};

    CameraParameters ExtractCameraParameters() const { return camera_parameters_; }

private:

    CameraCalibrationSettings calibration_settings_;
    std::vector<cv::Mat> calibration_images_;
    CameraParameters camera_parameters_;

    std::vector<std::vector<cv::Point3f>> reference_points_ { 1 };
    std::vector<std::vector<cv::Point2f>> real_points_;

    void CalculateReferenceGridPoints();
    void CalculateRealChessboardPoints();

    friend bool cv::findChessboardCorners(
        cv::InputArray image, 
        cv::Size patternSize, 
        cv::OutputArray corners, 
        int flags);

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


void UndistortPoint(const cv::Point2f&, cv::Point2f&, const CameraParameters&, const cv::Size&);


} // namespace camera_calibration

#endif
