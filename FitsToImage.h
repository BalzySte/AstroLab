#ifndef __FITSTOIMAGE_H__
#define __FITSTOIMAGE_H__

#include "FitsPhoto.h"
#include "settings.h"

#include <vector>

typedef unsigned char uchar;
void FitsTo8BitScale(std::vector<uchar>& grayImage,
				FitsPhoto& fitImage, pixelT bottom, pixelT top);

void FitsTo8BitScale(std::vector<uchar>& grayImage,
					 FitsPhoto& fitImage, bool reversed = false);


#endif // __FITSTOIMAGE_H__