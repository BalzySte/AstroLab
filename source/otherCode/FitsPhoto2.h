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


struct ImageRect
{
	int x1, y1;
	int x2, y2;
	
	ImageRect();
	ImageRect(int X1, int Y1, int X2, int Y2);
	
	int width() const;
	int height() const;
	void move(int xDispl, int yDispl);
	void setRect(int x1, int y1,int width, int height);
	ImageRect intersect(const ImageRect rect) const;
};


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

	// Private members access methods
	std::valarray<pixelT>& getImageArray();
	const std::valarray<pixelT>& getImageArray() const;
	std::string getHeaderStr();
	std::string getFileName();

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
	FitsPhoto extractMedianFiltered(int dim, bool processBorders = false) const;	// Median filter, returns a new FitsPhoto object
	FitsPhoto extractLowPassFilter3x3(bool processBorders == false) const;		// Should be implemented with n parameter
	
	// Image Useful data
	pixelT getImageMaxValue() const;
	pixelT getImageMinValue() const;
	
	// Image validity flag. (True if image was read/created and no error occured)
	bool isValid() const;
	
	
	// Position access methods
	int x() const;
	int y() const;
	void x(int x);
	void y(int y);

	// Valid area access functions.
	const ImageRect& validArea() const;
	ImageRect& validArea();


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
	
	ImageRect _validArea;
	// When applying some filters, image bbool processBordersorders may not be processed
	// or may be processed with an unreliable result.
	// The concept of validArea is that of keeping memory of the area
	// containing reliable and scientifically accurate data.
	// NOTE: Valid area coordinates are always referred to top left pixel
	// propagation of valid area at each operation is performed automatically
	// by operations and filters methods.
};


namespace
{
	inline ImageRect operationsValidArea(const FitsPhoto& image1, const FitsPhoto& image2);
}
	
#endif // __FitsPhoto__