#include "FitsToImage.h"
#include <cmath>

#include <iostream>

void FitsTo8BitScale(std::vector<uchar>& grayImage, FitsPhoto& 
					fitImage, pixelT bottom, pixelT top)
{
// 24 bit RGB colors can describe 16.777.216 different shades.
// Gray is obtained if R, G, and B channels have an equal intensity
// Different gray shades are obtained varying the intensity.
// Gray tonalities can therefore be described through an 8 bit number
// from 0 to 255. 0 is Black and 255 is white.

	int pixelNum = fitImage.getPixelNum();

	pixelT imageMin = fitImage.getImageMinValue();
	pixelT imageMax = fitImage.getImageMaxValue();
	
	pixelT stretch = std::abs(top - bottom);	
	
	pixelT pixelValue;

	if (top > bottom)
		for (int px = 0; px < pixelNum; ++px)
		{
			pixelValue = fitImage.getImageArray()[px];
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
		
	else if (top < bottom)		//Colors are reversed (max = black, min = white);
		for (int px = 0; px < pixelNum; ++px)
		{
			pixelValue = fitImage.getImageArray()[px];
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
		
	else
		for (int px = 0; px < pixelNum; ++px)
			grayImage[px] = 0.;
}


void FitsTo8BitScale(std::vector<uchar>& grayImage,
					 FitsPhoto& fitImage, bool reversed)
{
	// Overloaded version with default min - max stretch
	
	int pixelNum = fitImage.getPixelNum();
		
	pixelT imageMin = fitImage.getImageMinValue();
	pixelT imageMax = fitImage.getImageMaxValue();
	
	std::cout << imageMin << "  " << imageMax << std::endl;
	
	pixelT stretch = (imageMax - imageMin);
	
	if (reversed == false)
		for (int px = 0; px < pixelNum; ++px)
			grayImage[px] = static_cast<uchar> (
				(255*(fitImage.getImageArray()[px]-imageMin)/stretch));
	
	else if (reversed == true)
		for (int px = 0; px < pixelNum; ++px)
			grayImage[px] = 255 - static_cast<uchar> (
				(255*(fitImage.getImageArray()[px]-imageMin)/stretch));
}







