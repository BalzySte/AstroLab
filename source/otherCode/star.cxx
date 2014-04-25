#include "star.h"

#include <cmath>

bool star::operator< (star& otherStar)
{
	if (intensity < otherStar.intensity)
		return true;
	return false;
}

	
double star::xSigma()
{
	return sqrt(xVariance);
}


double star::xFWHM()
{
	return 2.355 * sqrt(xVariance);
}


double star::ySigma()
{
	return sqrt(yVariance);
}


double star::yFWHM()
{
	return 2.355 * sqrt(yVariance);
}


double star::areaFWHM()
{
}


bool comparePointObjectIntensity (pointObject a, pointObject b)
{
	if (a.intensity < b.intensity)
		return true;
	return false;
}