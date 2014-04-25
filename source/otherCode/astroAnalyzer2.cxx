#include "astroAnalyzer.h"

#include "star.h"
#include "settings.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

// Basic functions

std::vector<pointObject> crossMaximaDetection(const FitsPhoto& astroImage, double threshold,
					int crossPixelNum, bool scanEdges = true, bool respectValidArea = false)
{
	std::vector<pointObject> maxima;
	
	int width = ((respectValidArea == true) ? astroImage.getWidth() - validArea().x2 : astroImage.getWidth()) - crossPixelNum;
	int height = ((respectValidArea == true) ? astroImage.getHeight() - validArea().y2 : astroImage.getHeight()) - crossPixelNum;

	x = ((respectValidArea == true) ? astroImage.validArea().x1 : 0 ) + crossPixelNum;
	y = ((respectValidArea == true) ? astroImage.validArea().y1 : 0 ) + crossPixelNum; 

			
	for ( ; x < width; ++x)
		for ( ; y < height; ++y)
		{
			pixelT pixelValue = finalPhoto(x, y);
			
			// Should remove all negative intensity stars, since their "maximum" has
			// an intensity lower than the average background.
			if (pixelValue <= 0)
				continue;
			
			// Moves along 8 directions from central pixel and check if other pixels
			// are lower than central.
			
			bool noStar = false;
			
			// Checks \ dir
			for (int px = -crossPixelNum; px <= crossPixelNum; ++px)
				if (px != 0)
					if (pixelValue <= finalPhoto(x + px, y + px))
						noStar = true;
			
			// Checks / dir		
			for (int px = -crossPixelNum; px <= crossPixelNum; ++px)
				if (px != 0)
					if (pixelValue <= finalPhoto(x + px, y - px))
						noStar = true;
				
			// Checks -- dir
			for (int subX = -crossPixelNum; subX <= crossPixelNum; ++subX)
				if (subX != 0)
					if (pixelValue <= finalPhoto(x + subX, y))
						noStar = true;
					
			// Checks | dir
			for (int subY = -crossPixelNum; subY <= crossPixelNum; ++subY)
				if (subY != 0)
					if (pixelValue <= finalPhoto(x, y + subY))
						noStar = true;
			
			if (noStar == true)
				continue;
							
			// If cycle reaches this point, examined pixel must
			// be the centre of a star, therefore is added to list
			pointObject max(x, y, pixelValue);
			maxima.push_back(max);			// Should probably use emplace?
											// to construct directly in vector
											// instead of copying?

// 	if (scanEdges == true)
	// TODO: SHOULD CHECK FOR MAXIMA WITH INCOMPLETE STAR PATTERNS NEAR BORDERS	
	// Top border - corners included
	// Bottom border - corners included
	// Left border - no corners
	// Right border - no corners
	
	return stars;	
}


std::vector<pointObject> detectStars(const FitsPhoto& astroImage, int medianMatrixSize = 9, double threshold, bool respectValidArea = false)
{
	// Applying Low-pass 3x3 filter to astroImage
	FitsPhoto lowPassPhoto = astroImage.extractLowPassFilter3x3();
	
	// Applying Median filter to astroImage to obtain backgroundArray (default is 9x9)
	FitsPhoto backgroundFilteredPhoto = lowPassPhoto.extractMedianFiltered(medianMatrixSize);
	
	// Final image
	FitsPhoto finalPhoto = lowPassPhoto - backgroundFilteredPhoto;
	
	// Detects maxima
	std::vector<pointObject> stars = crossMaximaDetection(finalPhoto, threshold,
														  2, false, respectValidArea);
		
	std::sort(stars.begin(), stars.end(), comparePointObjectIntensity)
	
	pixelT maxIntensity = (stars.begin())->intensity * threshold;
	for (std::vector<star>::iterator it = stars.begin(); it != stars.end(); ++it)
		if (it->intensity < maxIntensity)
		{
			stars.erase(it, stars.end());
			break;
		}
		
	return stars;
}


std::vector<star> extractStarProfiles(const FitsPhoto& astroImage, int medianMatrixSize = 9, double threshold, bool respectValidArea = false)
{
	// Applying Low-pass 3x3 filter to astroImage
	FitsPhoto lowPassPhoto = astroImage.extractLowPassFilter3x3();
	
	// Applying Median filter to astroImage to obtain backgroundArray (default is 9x9)
	FitsPhoto backgroundFilteredPhoto = lowPassPhoto.extractMedianFiltered(medianMatrixSize);
	
	// Final image
	FitsPhoto finalPhoto = lowPassPhoto - backgroundFilteredPhoto;
	
	// Detects maxima
	std::vector<pointObject> stars = crossMaximaDetection(finalPhoto, threshold,
														  2, false, respectValidArea);
	
	std::sort(stars.begin(), stars.end(), comparePointObjectIntensity)
	
	pixelT maxIntensity = (stars.begin())->intensity * threshold;
	for (std::vector<star>::iterator it = stars.begin(); it != stars.end(); ++it)
		if (it->intensity < maxIntensity)
		{
			stars.erase(it, stars.end());
			break;
		}
		
	// Begins star analysis
	std::vector<star> processedStarVector;
	
	int maxStarRadius = 10;
	
	// For each star in vector:
	for (std::vector<star>::iterator it = starVector.begin(); it != starVector.end(); ++it)
	{		
		int xCentre = it->x;
		int yCentre = it->y;
		pixelT centreIntensity = it->intensity;
		
		bool validStar = true;
		
		double xAccurateCentre = 0;
		double yAccurateCentre = 0;
		double xSquareSum = 0;
		double ySquareSum = 0;
		
		// X axis
		
		pixelT xTotalCount = centreIntensity;
		int xDisplacement = -1;	// Displacement from centre, X direction		
		
		// left direction
		while (validStar)
		{
			int xCoord = xCentre + xDisplacement;
			
			// Checks whether star is good
			if (xCoord < medianMatrixSize/2 + maxStarRadius)
			{
				validStar = false;
				break;
			}
			
			// Scan radius = 10
			if  (xDisplacement < -maxStarRadius)
				break;
			
			pixelT pixelValue = finalPhoto(xCoord, yCentre);
			if (pixelValue < 0.)
				pixelValue = 0.;
			
			xAccurateCentre += pixelValue * xDisplacement;
			xSquareSum += pixelValue * pow(xDisplacement, 2);
			xTotalCount += pixelValue;
			
			--xDisplacement;
		}
		
		// right direction
		xDisplacement = 1;
		
		while (validStar)
		{
			int xCoord = xCentre + xDisplacement;
			
			// Checks whether star is good
			if (xCoord > width - medianMatrixSize/2 - maxStarRadius)
			{
				validStar = false;
				break;
			}
			
			// Scan radius = maxStarRadius
			if  (xDisplacement > maxStarRadius)
				break;
			
			pixelT pixelValue = finalPhoto(xCoord, yCentre);
			if (pixelValue < 0.)
				pixelValue = 0.;
			
			xAccurateCentre += pixelValue * xDisplacement;
			xSquareSum += pixelValue * pow(xDisplacement, 2);
			xTotalCount += pixelValue;
			
			++xDisplacement;
		}
		
		// Y final accurate coordinate.
		xAccurateCentre /= xTotalCount;
		it->xCentroid = xAccurateCentre;
		it->xFWHM = 2.355 * pow(xSquareSum/xTotalCount - pow(xAccurateCentre, 2), 0.5);
		
		// END X axis
		
		// Y axis
		
		pixelT yTotalCount = centreIntensity;
		int yDisplacement = -1;	// Displacement from centre, Y direction
		
		// left direction
		while (validStar)
		{
			int yCoord = yCentre + yDisplacement;
			
			// Checks whether star is good
			if (yCoord < medianMatrixSize/2 + maxStarRadius)
			{
				validStar = false;
				break;
			}
			
			// Scan radius = 10
			if  (yDisplacement < -maxStarRadius)
				break;
			
			pixelT pixelValue = finalPhoto(xCentre, yCoord);
			if (pixelValue < 0.)
				pixelValue = 0.;
			
			yAccurateCentre += pixelValue * yDisplacement;
			ySquareSum += pixelValue * pow(yDisplacement, 2);
			yTotalCount += pixelValue;
			
			--yDisplacement;
		}
		
		// Right direction
		yDisplacement = 1;
		
		while (validStar)
		{
			int yCoord = yCentre + yDisplacement;
			
			// Checks whether star is good
			if (yCoord > height - medianMatrixSize/2 - maxStarRadius)
			{
				validStar = false;
				break;
			}
			
			// Scan radius = 10
			if  (yDisplacement > maxStarRadius)
				break;
			
			pixelT pixelValue = finalPhoto(xCentre, yCoord);
			if (pixelValue < 0.)
				pixelValue = 0.;
			
			yAccurateCentre += pixelValue * yDisplacement;
			ySquareSum += pixelValue * pow(yDisplacement, 2);
			yTotalCount += pixelValue;
			
			++yDisplacement;
		}
		
		// Y final accurate coordinate.
		yAccurateCentre /= yTotalCount;
		it->yCentroid = yAccurateCentre;
		it->yFWHM = 2.355 * pow(ySquareSum/yTotalCount - pow(yAccurateCentre, 2), 0.5);
		
		// END Y axis
		
		if (validStar)
			processedStarVector.push_back(*it);
	}
	
	return processedStarVector;
}


// ATTENTION: N^2 complexity.
// Threshold is expressed in 
// void removeCollidingStars(const std::vector<star>& starsVector, double threshold)
// {
// 	for
// 		for
// 			if (distance < ...)
// 				
// }