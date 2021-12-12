#ifndef SECONDARY_STRUCTURES_H_
#define SECONDARY_STRUCTURES_H_

#include <string>


enum Button 
{
	ESC = 27,
	ENTER = 13,
	SPACE = 32
};

enum ExitStatus 
{
	SUCCESS = 0,
	FAILURE = -1
};

const std::string kKeys {
    "{help h   |        | print help                                           }"
	"{create c |        | create calibration settings file (json)              }"
	"{read r   | <none> | read calibration settings from specified file (json) }"
	"{number n |   15   | required minimum number of calibration images        }" 
};

std::string kMainWindowName { "Source" };

const int kFPS { 25 };


#endif
