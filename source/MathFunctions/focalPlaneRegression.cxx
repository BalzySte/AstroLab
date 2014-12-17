#include "focalPlaneRegression.h"

 
FocalPlane focalPlaneRegression(const std::vector<star> &starVector)
{
	int starNum = starVector.size();
	gsl_matrix *X = gsl_matrix_calloc(starNum, 3);
	gsl_vector *Y = gsl_vector_alloc(starNum);
	gsl_vector *beta = gsl_vector_alloc(3);
	gsl_vector *residuals = gsl_vector_alloc(starNum);
//	gsl_vector beta[3];

	int i = 0;
	for (std::vector<star>::const_iterator it=starVector.cbegin(); it != starVector.cend(); ++it)
	{
		gsl_vector_set(Y, i, (it->xFWHM + it->yFWHM) / 2);
		gsl_matrix_set(X, i, 0, 1);
		gsl_matrix_set(X, i, 1, it->x);
		gsl_matrix_set(X, i, 2, it->y);

		++i;
	}
 
	double chisq;
	double var_FWHM = 0;
	
	gsl_matrix *cov = gsl_matrix_alloc(3, 3);
	gsl_multifit_linear_workspace * wspc = gsl_multifit_linear_alloc(starNum, 3);
	gsl_multifit_linear(X, Y, beta, cov, &chisq, wspc);
	gsl_multifit_linear_residuals(X, Y, beta, residuals);
	
	for (int t = 0; t < starNum; ++t)
		var_FWHM += pow(gsl_vector_get(residuals, t), 2);
		
	var_FWHM /= starNum - 4;			// starNum - 3 - 1

	FocalPlane focalPlane;
	focalPlane.a = gsl_vector_get(beta, 1);
	focalPlane.b = gsl_vector_get(beta, 2);
	focalPlane.c = gsl_vector_get(beta, 0);
	focalPlane.var_FWHM = var_FWHM;
	
	gsl_matrix_free(X);
	gsl_matrix_free(cov);
	gsl_vector_free(Y);
	gsl_vector_free(beta);
	gsl_vector_free(residuals);
	gsl_multifit_linear_free(wspc);
	
	return focalPlane;
}