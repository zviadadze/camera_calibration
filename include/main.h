#ifndef MAIN_H_
#define MAIN_H_

#include <opencv2/core/types.hpp>
#include <string>


enum Button {
	SPACE = 32,
	ENTER = 13,
	ESC = 27
};

enum BoardType {
	CHESSBOARD = 0,
	SYMMETRIC_CIRCLE_GRID = 1,
	ASYMMETRIC_CIRCLE_GRID = 2
};

enum Status {
	GOOD = 0,
	BAD = -1
};

const int FPS { 25 };

std::string kMainWindow = "Main";


#endif
