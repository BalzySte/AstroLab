#ifndef __astroAnalizer_h__
#define __astroAnalizer_h__


#include <iostream>
#include <string>
#include <vector>
#include "FitsPhoto.h"

struct star
{
public:
	star();
	star(int xCoord,int yCoord, pixelT intensity);
	bool operator< (star otherStar) const;	// Compares intensities
	int x;
	int y;
	pixelT intensity;
	std::string print() const;
};


std::vector<star> detectStars(const FitsPhoto& astroImage, double treshold, int medianMatrixSize = 9);


#endif //__astroAnalizer_h__