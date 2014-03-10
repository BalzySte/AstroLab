// Astronomical Images Manipulation and data reduction

#include "astroAnalizer.h"
#include "settings.h"

#include <sstream>
#include <valarray>
#include <algorithm>
#define hystoResolution 1000
#define backgroundTreshold 10	// Percentage of total pixels (counted from lowest) that
								// should be used to evaluate background intensity

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
	/*
	for (int x = 1; x < width - 1; ++x)
		for (int y = 1; y < height - 1; ++y)
			imageArray[x + (y * width)] =
			(astroImage(x-1, y-1) + astroImage(x-1, y+1) + astroImage(x+1, y+1) + astroImage(x+1, y-1)) / 16	// Corner pixels
		+	(astroImage(x-1, y) + astroImage(x, y-1) + astroImage(x+1, y) + astroImage(x, y+1)) / 8		// N, S, E, W pixels
		+	astroImage(x, y) / 4;		// Central pixel
	*/	
		
	std::cout << "3x3 filter Applied" << std::endl;
	
//	std::valarray<pixelT> backgroundArray(0., pixelNumber); //Fill constructor. Efficiency?
	
	// Applying 9x9 Median filter to imageArray to obtain backgroundArray
	FitsPhoto backgroundFilteredPhoto = lowPassPhoto.extractMedianFiltered(medianMatrixSize);
/*	std::vector<pixelT> subArray;
	
	for (int x = 2; x < width - 2; ++x)
		for (int y = 2; y < height - 2; ++y)
		{
			for (int subX = -2; subX <= 2; ++subX)
				for (int subY = -2; subY <= 2; ++subY)
				{
					subArray.push_back(imageArray[(x + subX) + (y + subY) * width]);
				}
				
				std::sort(subArray.begin(), subArray.end());
				backgroundArray[x + (y * width)] = subArray[12];	// Median value
				subArray.clear();
		}
*/

	std::cout << "Background median filtered image created" << std::endl;
	
	// Final image
//	std::cout << "imageArray size: " << imageArray.size() << "backgroundArray size: " << backgroundArray.size() << std::endl;
	FitsPhoto finalPhoto = lowPassPhoto - backgroundFilteredPhoto;
//	std::valarray<pixelT> finalImage = imageArray - backgroundArray;
//	std::cout << "finalImage created, size: " << finalImage.size() << std::endl;
	
	std::vector<star> stars;
	
	for (int x = medianMatrixSize; x < width - medianMatrixSize; ++x)
		for (int y = medianMatrixSize; y < height - medianMatrixSize; ++y)
		{
//  		std::cout << x << 'x' << y << std::endl;
			bool isStar = true;
			pixelT pixelValue = finalPhoto(x, y);
			// Moves on 8 directions from central pixel and check if other pixels
			// are lower than central.
			
			// Should remove all negative intesity stars, since their "maximum" has
			// an itensity lower than the average background.
			if (pixelValue <= 0)
				isStar = false;
			
			// Checks \ dir
			for (int px = -2; px <= 2; ++px)
				if (px != 0)
					if (pixelValue <= finalPhoto(x + px, y + px))
					{
						isStar = false;
		//				std::cout << "\ dir false" << s)td::endl;
					}
			// Checks / dir		
			for (int px = -2; px <= 2; ++px)
				if (px != 0)
					if (pixelValue <= finalPhoto(x + px, y - px))
					{	
						isStar = false;
		//				std::cout << "/ dir false" << std::endl;
					}
				
			// Checks -- dir
			for (int subX = -2; subX <= 2; ++subX)
				if (subX != 0)
					if (pixelValue <= finalPhoto(x + subX, y))
					{	
						isStar = false;
	//					std::cout << "-- dir false" << std::endl;
					}
					
			// Checks | dir
			for (int subY = -2; subY <= 2; ++subY)
				if (subY != 0)
					if (pixelValue <= finalPhoto(x, y + subY))
					{	
						isStar = false;
//						std::cout << "| dir false" << std::endl;
					}
					
			/*		
			for (int subX = -1; subX <= 1; ++subX)
				for (int subY = -1; subY <= 1; ++subY)
					if (finalImage[(x + subX) + (y + subY) * width] < 
						(backgroundArray[(x + subX) + (y + subY) * width] * treshold)) // should average 9x9 background matrix
					{	
						isStar = false;
						std::cout << "background check" << std::endl;
					}
			*/		
					
		//	std::cin.ignore();
		//	std::cout << isStar << '\t';
					
			if (isStar == true)
			{
	//			std::cin.ignore();
				star starPos(x, y, pixelValue);
				stars.push_back(starPos);			// Should probably use emplace?
	//			std::cout << starPos.print() << std::endl;
			}
		}
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

/*
	pixelT minValue = getImageMinValue();
	pixelT maxValue = getImageMaxValue();
	pixelT stretch = maxValue - minValue;
	
	
	
	std::vector<int> hystogram(hystoResolution, 0);
	
	for (int px = 0; px < pixelNumber; ++px)
	{
		int bin = 1000*(imageArray[px] - minValue) / stretch;
		++(hystogram[bin]);
	}
	
	pixelT backgroundIntensity = 0;
	int pixelsCounted = 0;
	int binNum = 0;
	int binCount;
	
	while (pixelsCounted < pixelNumber*backgroundTreshold)
	{
		backgroundIntensity += hystogram[binNum];
		++pixelsCostd::string coordinates::print()
					unted;
		++binCount;
		if (binCount == hystogram[binNum])
		{
			binCount = 0;
			++binNum;
		}
	}
	
	backgroundIntensity /= pixelNumber*backgroundTreshold;
	imageArray -= backgroundIntensity;
*/
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