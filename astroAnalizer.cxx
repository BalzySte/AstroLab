// Astronomical Images Manipulation and data reduction

#include "astroAnalizer.h"
#include "settings.h"

#include <sstream>
#include <valarray>
#include <algorithm>

								
// Image should be clean of saturated pixels and calibrated.
std::vector<star> detectStars(const FitsPhoto& astroImage, double treshold, int medianMatrixSize)
{
	int width = astroImage.getWidth();
	int height = astroImage.getHeight();
	int pixelNumber = height * width;
	
	std::valarray<pixelT> imageArray(0., pixelNumber); //Fill constructor. Efficiency?
	
	std::cout << "parameters read, pxNum " << pixelNumber << ' ' << width << 'x' << height << std::endl;
	std::cout << "Valarray len " << imageArray.size() << std::endl;
	
	// Applying Low-pass 3x3 filter to imageArray
	FitsPhoto lowPassPhoto = astroImage.extractLowPassFilter3x3();
		
	std::cout << "3x3 filter Applied" << std::endl;
	
	// Applying Median filter to imageArray to obtain backgroundArray (default is 9x9)
	FitsPhoto backgroundFilteredPhoto = lowPassPhoto.extractMedianFiltered(medianMatrixSize);

	std::cout << "Background median filtered image created" << std::endl;
	
	// Final image
	FitsPhoto finalPhoto = lowPassPhoto - backgroundFilteredPhoto;
	
	std::vector<star> stars;
	
	for (int x = medianMatrixSize; x < width - medianMatrixSize; ++x)
		for (int y = medianMatrixSize; y < height - medianMatrixSize; ++y)
		{
			pixelT pixelValue = finalPhoto(x, y);
			
			// Should remove all negative intesity stars, since their "maximum" has
			// an itensity lower than the average background.
			if (pixelValue <= 0)
				continue;
			
			// Moves on 8 directions from central pixel and check if other pixels
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
							
			// If cycle reached this point, examined pixel must
			// be the centre of a star, therefore is added to list
			star starPos(x, y, pixelValue);
			stars.push_back(starPos);			// Should probably use emplace?
												// to construct directly in vector
												// instead of copying?
		}
		
	// Sorts stars according to intensity
	std::sort(stars.begin(), stars.end());
	
	pixelT brightestStar = (stars.end()-1)->intensity;
	std::cout << brightestStar << std::endl;
	for (std::vector<star>::iterator it = stars.end()-1; it != stars.begin()-1; --it)
		if ((it->intensity / brightestStar) < treshold)
		{
			stars.erase(stars.begin(), it);
			break;
		}
		
	for (std::vector<star>::const_iterator it = stars.cbegin(); it != stars.cend(); ++it)
		std::cout << it->print() << std::endl;
	std::cout << "DONE, with " << stars.size() << " Star(s)" << std::endl;

	return stars;
}



// star structure implementation

star::star()
{
}


star::star(int xCoord,int yCoord, pixelT value) : x(xCoord), y(yCoord), intensity(value)
{
}


std::string star::print() const
{
	std::stringstream stream;
	stream << '(' << x << ", " << y << ')' << "Value: " << intensity;
	return stream.str();
}


bool star::operator< (star otherStar) const
{
	if (intensity < otherStar.intensity)
		return true;
	return false;
}