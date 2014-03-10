#ifndef __astroAnalizer_h__
#define __astroAnalizer_h__


#include <iostream>
#include <string>

#include "FitsPhoto.h"

struct coordinates
{
public:
	coordinates();
	coordinates(int xCoord,int yCoord);
	int x;
	int y;
	std::string print() const;
};


void detectStars(const FitsPhoto& astroImage, double treshold);


#endif //__astroAnalizer_h__