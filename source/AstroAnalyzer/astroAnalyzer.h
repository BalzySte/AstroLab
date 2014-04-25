#ifndef __astroAnalizer_h__
#define __astroAnalizer_h__

#include <string>
#include <vector>
#include "FitsPhoto.h"
#include "settings.h"


// Astronomical Images Manipulation and data reduction


// ATTENTION
struct star
{
public:
	star();
	star(int xCoord,int yCoord, pixelT intensity);
	
	// Printing functions (produce strings)
	std::string print() const;
	std::string printDetailed() const;
	std::string printCoordinates() const;
	std::string printRelativeCentroids() const;
	std::string printFWHM() const;
	std::string printIntensity() const;
	
	bool operator< (star otherStar) const;	// Compares intensities
	
	int x;
	int y;
	double xCentroid;	// Precision centroid coordinates
	double yCentroid;	// Referred to center of (x, y) pixel
						// x and y centroid coordinates can therefore
						// vary between -0.5 and image width/height + 0.5
	double xFWHM;
	double yFWHM;
	pixelT intensity;
};


std::vector<star> detectStars(const FitsPhoto& astroImage, double threshold, int medianMatrixSize = 9);
std::vector<star> extractStarProfiles(const FitsPhoto& astroImage, double threshold, int medianMatrixSize = 9);


#endif //__astroAnalizer_h__