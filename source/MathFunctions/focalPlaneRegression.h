#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_multifit.h>
#include <vector>
#include "astroAnalyzer.h"


// Plane equation parameters
// z = ax + by + c
struct Plane
{
	double a;
	double b;
	double c;
};

struct FocalPlane : public Plane
{	
// 	double var_a;
// 	double var_b;
// 	double var_c;
	double var_FWHM;
};

FocalPlane focalPlaneRegression(const std::vector<star> &starVector);