#ifndef __SETTINGS_FILE__
#define __SETTINGS_FILE__


#define _PhotoArrayType_ double
#define stretchDockImagewidth 10
#define stretchResolution 1000

// This header file is meant to collect all program basic settings
// and default parameters. It is meant to be edited only by developers
// or expert programmers.

// Unsigned character type
typedef unsigned char uchar;

typedef _PhotoArrayType_ pixelT;

const char* const settingsFile = "/settings.ini";

// Defaults settings values:
// #######

// Stretch bar follows an exponential model which allow both
// little/precise correction and big/quick changes
// These 2 values represents the maximum increment (drawing bar to it's end)
// and the smallest correction (obtained with a single stretch bar step)
//extern double stretchBarMaxIncr;
//extern double stretchBarMinIncr;

//int stretchResolution = _StretchSlidersResolution_;

#endif // __SETTINGS_FILE__
