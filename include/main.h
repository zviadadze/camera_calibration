#ifndef MAIN_H_
#define MAIN_H_


#include <opencv2/core/types.hpp>
#include <string>


enum VideoSource {
	WEBCAM = 0
};

enum Buttons {
	SPACE = 32,
	ENTER = 13,
	ESC = 27
};

const int OK = 0;
const int FPS { 25 };

const cv::Size kCalibrationBoardSize = cv::Size(6, 4); // Количество точек соприкосновения квадратов калибровочной доски в длину и ширину cоответcтвенно.
const float kDistanceBetweenPoints = 0.0265f; // Длина cтороны квадрата на калибровочной доcке (в метрах).
const int kCalibrationImagesCount { 15 }; // Чем больше кол-во калибровочных изобрадений, тем точнее получаемые параметры калибровки.
const std::string kMainWindow { "Webcam" };
const std::string kCalibrationParametersFilename { CALIBRATION_PARAMETERS_FILENAME };


#endif