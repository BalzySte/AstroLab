#include "astroAnalizer.h"

#include "settings.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

								
// Image should be clean of saturated pixels and calibrated.
std::vector<star> detectStars(const FitsPhoto& astroImage, double threshold, int medianMatrixSize)
{
	int width = astroImage.getWidth();
	int height = astroImage.getHeight();
			
	// Applying Low-pass 3x3 filter to astroImage
	FitsPhoto lowPassPhoto = astroImage.extractLowPassFilter3x3();
			
	// Applying Median filter to astroImage to obtain backgroundArray (default is 9x9)
	FitsPhoto backgroundFilteredPhoto = lowPassPhoto.extractMedianFiltered(medianMatrixSize);
	
	// Final image
	FitsPhoto finalPhoto = lowPassPhoto - backgroundFilteredPhoto;
	
	std::vector<star> stars;
	
	for (int x = (medianMatrixSize/2) + 2 ; x < width - (medianMatrixSize/2) - 2; ++x)
		for (int y = (medianMatrixSize/2) + 2; y < height - (medianMatrixSize/2) - 2; ++y)
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
			for (int px = -2; px <= 2; ++px)
				if (px != 0)
					if (pixelValue <= finalPhoto(x + px, y + px))
						noStar = true;
			
			// Checks / dir		
			for (int px = -2; px <= 2; ++px)
				if (px != 0)
					if (pixelValue <= finalPhoto(x + px, y - px))
						noStar = true;
				
			// Checks -- dir
			for (int subX = -2; subX <= 2; ++subX)
				if (subX != 0)
					if (pixelValue <= finalPhoto(x + subX, y))
						noStar = true;
					
			// Checks | dir
			for (int subY = -2; subY <= 2; ++subY)
				if (subY != 0)
					if (pixelValue <= finalPhoto(x, y + subY))
						noStar = true;
			
			if (noStar == true)
				continue;
							
			// If cycle reaches this point, examined pixel must
			// be the centre of a star, therefore is added to list
			star starPos(x, y, pixelValue);
			stars.push_back(starPos);			// Should probably use emplace?
												// to construct directly in vector
												// instead of copying?
		}
		
	// Sorts stars according to intensity
	std::sort(stars.begin(), stars.end());
	
	// Throws away all stars less bright according to threshold
	pixelT brightestStar = (stars.end()-1)->intensity;
	for (std::vector<star>::iterator it = stars.end()-1; it != stars.begin()-1; --it)
		if ((it->intensity / brightestStar) < threshold)
		{
			stars.erase(stars.begin(), it);
			break;
		}
		
	return stars;
}



// star structure implementation

star::star()
{
}


star::star(int xCoord,int yCoord, pixelT value) :
	x(xCoord), y(yCoord), intensity(value)
{
}


std::string star::print() const
{
	std::stringstream stream;
	stream << '(' << x << ", " << y << ')' << "Value: " << intensity;
	return stream.str();
}


std::string star::printDetailed() const
{
	std::stringstream stream;
	stream << '(' << x << ", " << y << ") Value: " << intensity << " Centre: ("
		<< xCentroid << ", " << yCentroid << ") FWHM: (" << xFWHM << ", " << yFWHM << ')';
	return stream.str();
}


std::string star::printCoordinates() const
{
	std::stringstream stream;
	stream << '(' << x << ", " << y << ')';
	return stream.str();
}


std::string star::printRelativeCentroids() const
{
	std::stringstream stream;
	stream << '(' << xCentroid << ", " << yCentroid << ')';
	return stream.str();
}


std::string star::printFWHM() const
{
	std::stringstream stream;
	stream << '(' << xFWHM << ", " << yFWHM << ')';
	return stream.str();
}


std::string star::printIntensity() const
{
	std::stringstream value;
	// Checks if exponential notation is required
	if (fabs(intensity) < pow(10., 7) && fabs(intensity) > pow(10., -3)
		&& fabs(intensity) < pow(10., 7) && fabs(intensity) > pow(10., -3))
	{
		// Formats values strings
		int prec = static_cast<int>(std::min(3., 1000./fabs(intensity)));
		value << std::setprecision(prec) << std::fixed << intensity;
	}
	else
		// otherwise
	{
		//Numbers are shown using scientific notation
		value << std::setprecision(2) << std::scientific << intensity;
	}
	
	return value.str();
}


bool star::operator< (star otherStar) const
{
	if (intensity < otherStar.intensity)
		return true;
	return false;
}


std::vector<star> extractStarProfiles(const FitsPhoto& origAstroImage, double threshold, int medianMatrixSize)
{
	std::vector<star> starVector = detectStars(origAstroImage, threshold, medianMatrixSize);
	std::vector<star> processedStarVector;
	
	// Applying Low-pass 3x3 filter to astroImage
	FitsPhoto lowPassPhoto = origAstroImage.extractLowPassFilter3x3();
	
	// Applying Median filter to astroImage to obtain backgroundArray (default is 9x9)
	FitsPhoto backgroundFilteredPhoto = lowPassPhoto.extractMedianFiltered(medianMatrixSize);
	
	// Final image
	FitsPhoto astroImage = lowPassPhoto - backgroundFilteredPhoto;

	int width = astroImage.getWidth();
	int height = astroImage.getHeight();
	
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
			
			pixelT pixelValue = astroImage(xCoord, yCentre);
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
			
			pixelT pixelValue = astroImage(xCoord, yCentre);
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
			
			pixelT pixelValue = astroImage(xCentre, yCoord);
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
			
			pixelT pixelValue = astroImage(xCentre, yCoord);
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