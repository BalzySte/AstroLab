

struct imagePoint
{
public:
	int x, y;		// Position
};


struct pointObject : public imagePoint
{
public:
	pixelT value;
};


struct star : public pointObject
{
public:
	double xCentroid;
	double yCentroid;
	double xVariance;
	double yVariance;

	bool operator< (star& otherStar);
	double xSigma();
	double xVariance();
	double ySigma();
	double yVariance();
	double areaFWHM();
}


struct photometryStar : public star
{
public:
	pixelT totalIntensity;
	int countedPixels;
}


// Used by std::sort
bool comparePointObjectIntensity (pointObject a, pointObject b);