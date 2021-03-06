#include "FitsToImage.h"


void FitsTo8BitScale(std::vector<uchar>& grayImage, FitsPhoto& 
					fitImage, pixelT bottom, pixelT top)
{
// 24 bit RGB colours can describe 16.777.216 different colours.
// R, G, B are 8 bit unsigned numbers between 0 and 255.
// Gray is obtained if R, G, and B channels have equal intensity.
// Different shades of gray are obtained by varying the intensity.
// Aforementioned shades can therefore be described by an 8 bit number
// included between 0 and 255. 0 is Black and 255 is White.

	int pixelNum = fitImage.getPixelNum();
	
	pixelT stretch = std::abs(top - bottom);	
	std::valarray<pixelT> &imgArrayRef = fitImage.getImageArray();
	
	pixelT pixelValue;

	if (top > bottom)
		for (int px = 0; px < pixelNum; ++px)
		{
			pixelValue = imgArrayRef[px];
			if (pixelValue >= top)
			{
				grayImage[px] = 255;
				continue;
			}
			if (pixelValue <= bottom)
			{
				grayImage[px] = 0;
				continue;
			}
			
			grayImage[px] = static_cast<uchar> (
				(255*(pixelValue - bottom) / stretch));
		}
		
	//Colors are reversed (max = black, min = white);
	else if (top < bottom)
		for (int px = 0; px < pixelNum; ++px)
		{
			pixelValue = imgArrayRef[px];
			if (pixelValue <= top)
			{
				grayImage[px] = 255;
				continue;
			}
			if (pixelValue >= bottom)
			{
				grayImage[px] = 0;
				continue;
			}
			
			grayImage[px] = 255 - static_cast<uchar> (
				(255*(pixelValue - top) / (stretch)));
		}
	
	// If top == bottom returns an empty image
	else
		for (int px = 0; px < pixelNum; ++px)
			grayImage[px] = 0.;
}


// Overloaded version with default min - max stretch
void FitsTo8BitScale(std::vector<uchar>& grayImage,
					 FitsPhoto& fitImage, bool reversed)
{	
	int pixelNum = fitImage.getPixelNum();
		
	pixelT imageMin = fitImage.getImageMinValue();
	pixelT imageMax = fitImage.getImageMaxValue();
		
	pixelT stretch = (imageMax - imageMin);
	std::valarray<pixelT> &imgArrayRef = fitImage.getImageArray();
	
	if (reversed == false)
		for (int px = 0; px < pixelNum; ++px)
			grayImage[px] = static_cast<uchar> (
				(255*(imgArrayRef[px]-imageMin)/stretch));
	
	else if (reversed == true)
		for (int px = 0; px < pixelNum; ++px)
			grayImage[px] = 255 - static_cast<uchar> (
				(255*(imgArrayRef[px]-imageMin)/stretch));
}







