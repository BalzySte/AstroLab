//		+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
//		| 							FitsPhoto                           |
//		+  AUTHOR:  Stefano Balzan		MAIL:  stefano.balzan@gmail.com	+
//		|  DATE: Feb 2014				BASED ON:  NASA CCFits Lib 2.4	|
//		+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

// Easy manipulation of 2D Array FITS FILE containing Astronomical Images
// This library is built on CCfits library and aims to offer a simplified and complete
// layer for image manipulation.
//
// This library can open fits, create new ones and save them.
// It manipulates only the Primary Data Header Unit, as it's
// the only useful one for the project purposes.

#ifndef __FitsPhoto__
#define __FitsPhoto__

#include <valarray>
#include "settings.h"


class FitsPhoto
{

public:
	// Constructors / Destructors
	FitsPhoto();
	//ADD DESTRUCTOR ???

	// File managing methods (Open/New/Save)
	void open(const std::string &filename);
	void create(int width, int height, int xPos = 0, int yPos = 0);
	void write(const std::string &filename, bool overwrite);
	
	//TODO: Add methods for closing / flushing

	// Private members access methods
	std::valarray<pixelT>& getImageArray();
	const std::valarray<pixelT>& getImageArray() const;
	std::string getHeaderStr();
	std::string getFileName() const;

	// Image keywords access
	int getBitpix() const;
	int getWidth() const;
	int getHeight() const;
	int getPixelNum() const;
	
	// (X,Y) Coordinates access operator
	pixelT& operator() (int x, int y);
	const pixelT& operator() (int x, int y) const;

	// Basic Operations with FITS Images
	void move(int xShift, int yShift);
	FitsPhoto operator+ (const FitsPhoto& photo) const;
	FitsPhoto operator- (const FitsPhoto& photo) const;
	FitsPhoto operator* (const FitsPhoto& photo) const;
	FitsPhoto operator/ (const FitsPhoto& photo) const;
	FitsPhoto operator* (double factor) const;
	FitsPhoto operator/ (double divisor) const;
	FitsPhoto& operator+= (const FitsPhoto& photo);
	FitsPhoto& operator-= (const FitsPhoto& photo);
	FitsPhoto& operator*= (double factor);
	FitsPhoto& operator/= (double divisor);
	
	// Filters
	FitsPhoto extractMedianFiltered(int dim) const;	// Median filter, returns a new FitsPhoto object
	FitsPhoto extractLowPassFilter3x3() const;		// Should be implemented with n parameter
	
	// Image Useful data
	pixelT getImageMaxValue() const;
	pixelT getImageMinValue() const;
	pixelT getImageMeanValue() const;
	
	// Image validity flag. (True if image was read/created and no error occured)
	bool isValid() const;


private:
	bool _valid;				// Validity flag
	int _width;					// Image Width
	int _height;				// Image Height
	int _pixelNumber;			// Pixel count (Width * Height)
	int _bitpix;				// Bitpix 
	std::valarray<pixelT> _imageArray;  // Pixels value array
	std::string _filename;

	int _x;
	int _y;
};

#endif // __FitsPhoto__
