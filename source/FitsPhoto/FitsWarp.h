#ifndef __FitsWarp__
#define __FitsWarp__

#include "FitsPhoto.h"

namespace FitsManip
{
	
FitsPhoto polarWarp(const FitsPhoto& fitsImage, int xCenter, int yCenter);

}
#endif // __FitsWarp__