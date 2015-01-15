#include <iomanip>
#include <sstream>
#include "functions.h"

QString printPixelT(pixelT value)
{
	std::stringstream valueStr;

	// If max and min are in a range suitable for fixed representation
	if (fabs(value) < pow(10., 7) && fabs(value) > pow(10., -3))
	{
		// Calculates maximum number of decimals
		int prec = static_cast<int>(std::min(3., 1000./fabs(value)));
		// Formats values strings
		valueStr << std::setprecision(prec) << std::fixed << value;
	}
	else // otherwise
	{
		//Numbers are shown using scientific notation
		valueStr << std::setprecision(1) << std::scientific << value;
	}

	return QString::fromStdString(valueStr.str());
}


int signum (int x)
{
	return (x > 0) - (x < 0);
}