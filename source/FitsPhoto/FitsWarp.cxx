#include "FitsWarp.h"

#include <cmath>
#include <iostream>


FitsPhoto FitsManip::polarWarp(const FitsPhoto& fitsImage, int xCenter, int yCenter)
{
	int fitsImageWidth = fitsImage.getWidth();
	int fitsImageHeight = fitsImage.getHeight();
	// R max, evaluating maximum radius needed to cover the entire image rectangle

	// Distances of (xCenter, yCenter) from image corners.
	/*double distances[4] = 
	{
		sqrt(pow(0 - xCenter, 2) + pow(0 - yCenter, 2)),
		sqrt(pow(fitsImageWidth - xCenter, 2) + pow(0 - yCenter, 2)),
		sqrt(pow(fitsImageWidth - xCenter, 2) + pow(fitsImageHeight - yCenter, 2)),
		sqrt(pow(0 - xCenter, 2) + pow(fitsImageHeight - yCenter, 2))
	};
	
	double rMaxD = 0.;
	for (int i = 0; i < 4; ++i)
		if (distances[i] > rMaxD)
			rMaxD = distances[i];
		
	int rMax = static_cast<int>(rMaxD);
	*/
	double rMaxD = 0.;
	rMaxD = std::max(rMaxD, sqrt(pow(0 - xCenter, 2) + pow(0 - yCenter, 2)));
	rMaxD = std::max(rMaxD, sqrt(pow(fitsImageWidth - xCenter, 2) + pow(0 - yCenter, 2)));
	rMaxD = std::max(rMaxD, sqrt(pow(fitsImageWidth - xCenter, 2) + pow(fitsImageHeight - yCenter, 2)));
	rMaxD = std::max(rMaxD, sqrt(pow(0 - xCenter, 2) + pow(fitsImageHeight - yCenter, 2)));
	
	int rMax = static_cast<int>(rMaxD);
	
	// Evaluating theta steps needed to approximately cover all pixels at rMax
	int thetaSteps = static_cast<int>(rMax * 2 * M_PI);
	
	// New polar image is created
	FitsPhoto polarImage;
	polarImage.create(thetaSteps, rMax);

/*	for (int j = 0; j < thetaSteps; ++j)
		for (int k = 0; k < rMax; ++k)
			polarImage(j, k) = 0.;
*/		
		
	// New polar plot image is filled
	
	// (0, theta) for each theta represents the same point (xCenter, yCenter)
	for (int theta = 0; theta < thetaSteps; ++theta)
		polarImage(theta, 0) = fitsImage(xCenter, yCenter);

	// (r, theta) with r != 0 and 0 < theta < 2 PI 
	// Checks if image 
	for (int r = 1; r < rMax; ++r)
		for(int theta = 0; theta < thetaSteps; ++theta)
		{
			// coordinates are approximated to nearest pixel and if they map an
			// existing pixel in fitsImage its value is assigned to the new one
			int x = static_cast<int>(r*cos(2*M_PI*theta/thetaSteps) + xCenter + 0.5);
			int y = static_cast<int>(r*sin(2*M_PI*theta/thetaSteps) + yCenter + 0.5);
// 			std::cout << x << ' ' << y << "->" << r << ' ' << theta << std::endl;
			if (x > 0 && x < fitsImageWidth && y > 0 && y < fitsImageHeight)
// 				std::cout << polarImage(theta, r) << std::endl;
				polarImage(theta, r) = fitsImage(x, y);
// 			std::cout << "pass" << std::endl;

		}
		std::cout << "pass" << std::endl;

	// All pixels not set by the two cycles above are set to 0. 
	
	return polarImage;
}