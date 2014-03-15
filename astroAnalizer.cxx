#include "astroAnalizer.h"

#include "settings.h"
#include <sstream>
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
	
	for (int x = medianMatrixSize; x < width - medianMatrixSize; ++x)
		for (int y = medianMatrixSize; y < height - medianMatrixSize; ++y)
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
						noStar = true;;
				
			// Checks -- dir
			for (int subX = -2; subX <= 2; ++subX)
				if (subX != 0)
					if (pixelValue <= finalPhoto(x + subX, y))
						noStar = true;;
					
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



bool star::operator< (star otherStar) const
{
	if (intensity < otherStar.intensity)
		return true;
	return false;
}


void extractStarProfiles (const FitsPhoto& astroImage, const FitsPhoto& background, std::vector<star>& starVector)
{
	// For each star in vector:
	for (std::vector<star>::iterator it = starVector.begin(); it != starVector.end(); ++it)
	{
		// Starts from central maximum pixel
		int xPixelDispl = 0;	// Displacement from centre, X direction
		int yPixelDispl = 0;	// Displacement from centre, Y direction
		
		int xCentre = it->x;
		int yCentre = it->y;
		
		pixelT centreIntensity = it->intensity;
		
		pixelT xTotalCount = 0;
		pixelT yTotalCount = 0;
		
		double xAccurateCentre = 0.;
		double yAccurateCentre = 0.;
		
		pixelT xSquareCount = 0.;
		pixelT ySquareCount = 0.;
		
		// Calculates mean and mean squares along two axes
		// X and Y coordinates means will be the coordinates of the centroid
		// FWHM along 2 axes will be extracted from  VAR(X) = E(X)^2 - E(X^2).
		
		// X axis, left direction (central pixel included)
		while (true)
		{
			pixelT xPixelCount = astroImage(xCentre + xPixelDispl, yCentre);
			pixelT backgroundIntensity = background(xCentre + xPixelDispl, yCentre);
			xAccurateCentre += xPixelCount * xPixelDispl;
			xTotalCount += xPixelCount;
			xSquareCount += pow(xPixelCount, 2);
			
			if ((xPixelCount/backgroundIntensity) < 2. || xPixelCount < centreIntensity/100.)
				break;
			
			--xPixelDispl;
		//find centroid and FWHM on x/y
		}
		
		// X axis, right direction (central pixel excluded)
		xPixelDispl = 1;
		while (true)
		{
			pixelT xPixelCount = astroImage(xCentre + xPixelDispl, yCentre);
			pixelT backgroundIntensity = background(xCentre + xPixelDispl, yCentre);
			xAccurateCentre += xPixelCount * xPixelDispl;
			xTotalCount += xPixelCount;
			xSquareCount += pow(xPixelCount, 2);
			
			if ((xPixelCount/backgroundIntensity) < 2. || xPixelCount < centreIntensity/100.)
				break;
			
			++xPixelDispl;
		}
		
		
		// X final accurate coordinate.
		xAccurateCentre /= xTotalCount;
		it->xFWHM = pow(xAccurateCentre, 2) - xSquareCount/xTotalCount;
		
		// Y axis, up direction (central pixel included)
		while (true)
		{
			pixelT yPixelCount = astroImage(xCentre, yCentre + yPixelDispl);
			pixelT backgroundIntensity = background(xCentre, yCentre + yPixelDispl);
			yAccurateCentre += yPixelCount * yPixelDispl;
			yTotalCount += yPixelCount;
			ySquareCount += pow(yPixelCount, 2);
			
			if ((yPixelCount/backgroundIntensity) < 2. || yPixelCount < centreIntensity/100.)
				break;
			
			--yPixelDispl;
			//find centroid and FWHM on x/y
		}
		
		// Y axis, down direction (central pixel excluded)
		yPixelDispl = 1;
		while (true)
		{
			pixelT yPixelCount = astroImage(yCentre, yCentre + yPixelDispl);
			pixelT backgroundIntensity = background(yCentre, yCentre + yPixelDispl);
			yAccurateCentre += yPixelCount * yPixelDispl;
			yTotalCount += yPixelCount;
			ySquareCount += pow(yPixelCount, 2);
			
			if ((yPixelCount/backgroundIntensity) < 2. || yPixelCount < centreIntensity/100.)
				break;
			
			++yPixelDispl;
		}
		
		// Y final accurate coordinate.
		yAccurateCentre /= yTotalCount;
		it->xFWHM = pow(yAccurateCentre, 2) - ySquareCount/yTotalCount;
		
		it->xCentroid = xAccurateCentre;
		it->yCentroid = yAccurateCentre;
	}
}