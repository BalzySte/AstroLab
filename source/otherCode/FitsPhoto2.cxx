#include "FitsPhoto.h"

#include <CCfits>
#include <algorithm>
#include <sstream>

// Quickselect ANSI C function for median filter
extern "C"
{
	#include "quickselect.c"
}

// Default Constructor, sets image position to default value (0,0)
// Unless performing particular operations, such as aligning images
// and adding them up, position is not relevant and must be set
// to (0,0).
FitsPhoto::FitsPhoto() : _x(0), _y(0), _filename("")
{
	_valid = false;
}

void FitsPhoto::open(const std::string &filename)
{
	_filename = filename;

	CCfits::FITS inFitsFile(filename, CCfits::Read, true);
	CCfits::PHDU& primaryHDU = inFitsFile.pHDU();

	primaryHDU.readAllKeys();
	primaryHDU.read(_imageArray);

	// reading FITS keywords
	int axisNum = static_cast<int> (primaryHDU.axes());
	if (axisNum != 2)	//Checks if reading a bi-dimensional array
		return;

	_bitpix = static_cast<int> (primaryHDU.bitpix());
	_width = static_cast<int> (primaryHDU.axis(0));
	_height = static_cast<int> (primaryHDU.axis(1));

//  TODO: Import other non essential keywords (i.e.: EXPOSURE, CCDTEMP)
//	primaryHDU.keyWord("NAXIS").value<int>(_bitpix);
//	primaryHDU.readKey<int>("BITPIX", _bitpix);
//	primaryHDU.readKey<int>("NAXIS1", _width);
//	primaryHDU.readKey<int>("NAXIS2", _height);
	
	_pixelNumber = _width * _height;
	
	_valid = true;	//File opened and read, setting validity flag
	
	_validArea = ImageRect(0, 0, _width - 1, _height - 1);

	inFitsFile.~FITS();	//Fits object is destroyed
}


void FitsPhoto::create(int width, int height, int xPos, int yPos)
{
	_bitpix = -8*(sizeof(pixelT));
	_width = width;
	_height = height;
	_pixelNumber = width*height;
	_imageArray.resize(_pixelNumber);	// Added pixel values are initialised to pixelT()
										// then to float() or double() which is 0.
	_x = xPos;
	_y = yPos;
	
	_validArea = ImageRect(0, 0, _width - 1, _height - 1);
}


void FitsPhoto::write(const std::string &filename, bool overwrite)
{
	_filename = filename;
	if (overwrite == true)
		_filename.insert(0, "!");

	long axes[2] = {_width, _height};
	CCfits::FITS outFitsFile (_filename, _bitpix, 2, axes);
	CCfits::PHDU& primaryHDU = outFitsFile.pHDU();
	primaryHDU.write<pixelT>(1, _pixelNumber, _imageArray);
	outFitsFile.~FITS();

	_valid = true;  
}


std::string FitsPhoto::getHeaderStr()
{
	//Fills Header String
	std::stringstream hstream;
	hstream << "FitsPhoto 2D Image object" << std::endl << "PrimaryHDU:: Bitpix: "
			<< _bitpix << " naxis = 2" << std::endl << "Axis Lengths:" << std::endl
			<< " axis[0] " << _width << std::endl <<  " axis[1] " << _height << std::endl;

	return hstream.str();	
}


int FitsPhoto::getBitpix() const
{
	return _bitpix;
}


int FitsPhoto::getWidth() const
{
	return _width;
}


int FitsPhoto::getHeight() const
{
	return _height;
}


int FitsPhoto::getPixelNum() const
{
	return _pixelNumber;
}


std::valarray<pixelT>& FitsPhoto::getImageArray()
{
	return _imageArray;
}


const std::valarray<pixelT>& FitsPhoto::getImageArray() const
{
	return _imageArray;
}


std::string FitsPhoto::getFileName()
{
	return _filename;
}


bool FitsPhoto::isValid() const
{
	return _valid;
}


// (X,Y) Coordinates access methods
// ATTENTION: No control on bounds. It's because of performance, not laziness
pixelT& FitsPhoto::operator() (int x, int y)
{
	return _imageArray[x + (_width * y)];
}


const pixelT& FitsPhoto::operator() (int x, int y) const
{
	return _imageArray[x + (_width * y)];
}



// Basic Operations with FITS Images

void FitsPhoto::move(int xShift, int yShift)
{
	_x += xShift;
	_y += yShift;
}


FitsPhoto FitsPhoto::operator+ (const FitsPhoto& photo) const
{
	FitsPhoto sumImage;

	int xSumPos;
	int xDist;
	int ySumPos;
	int yDist;
	int sumWidth;
	int sumHeight;

	if (_x <= photo._x)
	{
		xSumPos = _x;
		xDist = photo._x - _x;
		sumWidth = std::max<int>(_width, photo._width + xDist);
	}
	else
	{
		xSumPos = photo._x;
		xDist = _x - photo._x;
		sumWidth = std::max<int>(photo._width, _width + xDist);
	}

	if (_y <= photo._y)
	{
		ySumPos = _y;
		yDist = photo._y - _y;
		sumHeight = std::max<int>(_height, photo._height + yDist);
	}
	else
	{
		ySumPos = photo._y;
		yDist = _y - photo._y;
		sumHeight = std::max<int>(photo._height, _height + yDist);
	}

	sumImage.create(sumWidth, sumHeight, xSumPos, ySumPos);

	int x_dst = _x - xSumPos;		//x distance of 1st image reference from sumImage reference
	int y_dst = _y - ySumPos;		//y distance of 1st image reference from sumImage reference
	for (int col = 0; col < _width; ++col)
		for (int line = 0; line < _height; ++line)
			sumImage.getImageArray()[(col+x_dst) + ((line+y_dst) * sumWidth)] +=
						getImageArray()[col + (line * _width)];

	x_dst = photo._x - xSumPos;		//x distance of 2nd image reference from sumImage reference
	y_dst = photo._y - ySumPos;		//y distance of 2nd image reference from sumImage reference
	for (int col = 0; col < photo._width; ++col)
		for (int line = 0; line < photo._height; ++line)
			sumImage.getImageArray()[(col+x_dst) + ((line+y_dst) * sumWidth)] +=
						photo.getImageArray()[col + (line * photo._width)];

	return sumImage;
}


FitsPhoto FitsPhoto::operator- (const FitsPhoto& photo) const
{
	FitsPhoto subtrImage;

	int xSubtrPos;
	int xDist;
	int ySubtrPos;
	int yDist;
	int subtrWidth;
	int subtrHeight;

	if (_x <= photo._x)
	{
		xSubtrPos = _x;
		xDist = photo._x - _x;
		subtrWidth = std::max<int>(_width, photo._width + xDist);
	}
	else
	{
		xSubtrPos = photo._x;
		xDist = _x - photo._x;
		subtrWidth = std::max<int>(photo._width, _width + xDist);
	}

	if (_y <= photo._y)
	{
		ySubtrPos = _y;
		yDist = photo._y - _y;
		subtrHeight = std::max<int>(_height, photo._height + yDist);
	}
	else
	{
		ySubtrPos = photo._y;
		yDist = _y - photo._y;
		subtrHeight = std::max<int>(photo._height, _height + yDist);
	}

	subtrImage.create(subtrWidth, subtrHeight, xSubtrPos, ySubtrPos);

	int x_dst = _x - xSubtrPos;		//x distance of 1st image reference from subtrImage reference
	int y_dst = _y - ySubtrPos;		//y distance of 1st image reference from subtrImage reference
	for (int col = 0; col < _width; ++col)
		for (int line = 0; line < _height; ++line)
			subtrImage.getImageArray()[(col+x_dst) + ((line+y_dst) * subtrWidth)] +=
						getImageArray()[col + (line * _width)];

	x_dst = photo._x - xSubtrPos;		//x distance of 2nd image reference from subtrImage reference
	y_dst = photo._y - ySubtrPos;		//y distance of 2nd image reference from subtrImage reference
	for (int col = 0; col < photo._width; ++col)
		for (int line = 0; line < photo._height; ++line)
			subtrImage.getImageArray()[(col+x_dst) + ((line+y_dst) * subtrWidth)] -=
						photo.getImageArray()[col + (line * photo._width)];

	return subtrImage;
}


FitsPhoto FitsPhoto::operator* (double factor) const
{
	FitsPhoto multipImage;
	multipImage.create(_width, _height);

	multipImage._imageArray = _imageArray * factor;

	return multipImage;
}


FitsPhoto FitsPhoto::operator* (const FitsPhoto& photo) const
{
	FitsPhoto prodImage;
	
	int xProdPos;
	int xDist;
	int yProdPos;
	int yDist;
	int prodWidth;
	int prodHeight;
	
	if (_x <= photo._x)
	{
		xProdPos = _x;
		xDist = photo._x - _x;
		prodWidth = std::max<int>(_width, photo._width + xDist);
	}
	else
	{
		xProdPos = photo._x;
		xDist = _x - photo._x;
		prodWidth = std::max<int>(photo._width, _width + xDist);
	}
	
	if (_y <= photo._y)
	{
		yProdPos = _y;
		yDist = photo._y - _y;
		prodHeight = std::max<int>(_height, photo._height + yDist);
	}
	else
	{
		yProdPos = photo._y;
		yDist = _y - photo._y;
		prodHeight = std::max<int>(photo._height, _height + yDist);
	}
	
	prodImage.create(prodWidth, prodHeight, xProdPos, yProdPos);
	
	int x_dst = _x - xProdPos;		//x distance of 1st image reference from prodImage reference
	int y_dst = _y - yProdPos;		//y distance of 1st image reference from prodImage reference
	for (int col = 0; col < _width; ++col)
		for (int line = 0; line < _height; ++line)
			prodImage.getImageArray()[(col+x_dst) + ((line+y_dst) * prodWidth)] +=
			getImageArray()[col + (line * _width)];
		
		x_dst = photo._x - xProdPos;		//x distance of 2nd image reference from prodImage reference
		y_dst = photo._y - yProdPos;		//y distance of 2nd image reference from prodImage reference
		for (int col = 0; col < photo._width; ++col)
			for (int line = 0; line < photo._height; ++line)
				prodImage.getImageArray()[(col+x_dst) + ((line+y_dst) * prodWidth)] *=
				photo.getImageArray()[col + (line * photo._width)];
			
			return prodImage;
}


FitsPhoto FitsPhoto::operator/ (const FitsPhoto& photo) const
{
	FitsPhoto divImage;
	
	int xDivPos;
	int xDist;
	int yDivPos;
	int yDist;
	int divWidth;
	int divHeight;
	
	if (_x <= photo._x)
	{
		xDivPos = _x;
		xDist = photo._x - _x;
		divWidth = std::max<int>(_width, photo._width + xDist);
	}
	else
	{
		xDivPos = photo._x;
		xDist = _x - photo._x;
		divWidth = std::max<int>(photo._width, _width + xDist);
	}
	
	if (_y <= photo._y)
	{
		yDivPos = _y;
		yDist = photo._y - _y;
		divHeight = std::max<int>(_height, photo._height + yDist);
	}
	else
	{
		yDivPos = photo._y;
		yDist = _y - photo._y;
		divHeight = std::max<int>(photo._height, _height + yDist);
	}
	
	divImage.create(divWidth, divHeight, xDivPos, yDivPos);
	
	int x_dst = _x - xDivPos;		//x distance of 1st image reference from divImage reference
	int y_dst = _y - yDivPos;		//y distance of 1st image reference from divImage reference
	for (int col = 0; col < _width; ++col)
		for (int line = 0; line < _height; ++line)
			divImage.getImageArray()[(col+x_dst) + ((line+y_dst) * divWidth)] +=
			getImageArray()[col + (line * _width)];
		
	x_dst = photo._x - xDivPos;		//x distance of 2nd image reference from divImage reference
	y_dst = photo._y - yDivPos;		//y distance of 2nd image reference from divImage reference
	for (int col = 0; col < photo._width; ++col)
		for (int line = 0; line < photo._height; ++line)
			divImage.getImageArray()[(col+x_dst) + ((line+y_dst) * divWidth)] /=
			photo.getImageArray()[col + (line * photo._width)];
		
		return divImage;
}


FitsPhoto FitsPhoto::operator/ (double divisor) const
{
	FitsPhoto divImage;
	divImage.create(_width, _height);

	divImage._imageArray = _imageArray / divisor;

	return divImage;
}


FitsPhoto& FitsPhoto::operator+= (const FitsPhoto& photo)
{
	FitsPhoto temp;
	temp = *this + photo;
	*this = std::move(temp);		// Good idea???

	return *this;
}


FitsPhoto& FitsPhoto::operator-= (const FitsPhoto& photo)
{
	FitsPhoto temp;
	temp = *this - photo;
	*this = std::move(temp);		// Good idea???

	return *this;
}


FitsPhoto& FitsPhoto::operator*= (double factor)
{
	_imageArray *= factor;

	return *this;
}


FitsPhoto& FitsPhoto::operator/= (double divisor)
{
	_imageArray /= divisor;

	return *this;
}


pixelT FitsPhoto::getImageMaxValue() const
{
	return _imageArray.max();
}


pixelT FitsPhoto::getImageMinValue() const
{	
	return _imageArray.min();
}


// Filters
// ATTENTION: FUNCTION IS OPTIMISED FOR SMALL ARBITRARY KERNELS
// 			  SHOULD IMPLEMENT FILTER SPECIFIC FOR EACH SMALL
//			  SPECIFIC VALUE AND AN HISTOGRAM ALGORITHM FOR LARGE KERNELS
FitsPhoto FitsPhoto::extractMedianFiltered(int dim, bool processBorders) const
{
	int n = (dim - 1) / 2;
	
	// ATTENTION: With this implementation borders are not processed
	
	FitsPhoto medianImage;
	medianImage.create(_width, _height);	//Image position is not reported but instead set to (0,0)
	int cellsNumber = dim*dim;

	
	pixelT *subArray = new pixelT[cellsNumber];
// 	std::vector<pixelT> subArray;

	if (processBorders == false)
	{
		for (int x = n; x < _width - n; ++x)
			for (int y = n; y < _height - n; ++y)
			{
				int index = 0;
				for (int subX = -n; subX <= n; ++subX)
					for (int subY = -n; subY <= n; ++subY)
					{
						subArray[index] = _imageArray[(x + subX) + (y + subY) * _width];
						++index;
					}
					
				medianImage._imageArray[x + (y * _width)] = quick_select(subArray, cellsNumber);
			}
		
		medianImage._validArea = ImageRect(_validArea.x1 + n, _validArea.y1 + n,
										_validArea.x2 - 1 - n, _validArea.y2 - 1 - n);
	

	} /*else {
	// TODO: Median filter with border processing
	}*/
		
	delete[] subArray;
	
	return medianImage;
}


FitsPhoto FitsPhoto::extractLowPassFilter3x3(bool processBorders) const
{
	if (processBorders == false)
	{
		FitsPhoto filteredImage;
		filteredImage.create(_width, _height);
		
		for (int x = 1; x < _width - 1; ++x)
			for (int y = 1; y < _height - 1; ++y)
				filteredImage._imageArray[x + (y * _width)] =
				(operator()(x-1, y-1) + operator()(x-1, y+1) + operator()(x+1, y+1) + operator()(x+1, y-1)) / 16	// Corner pixels
			+	(operator()(x-1, y) + operator()(x, y-1) + operator()(x+1, y) + operator()(x, y+1)) / 8		// N, S, E, W pixels
			+	(operator()(x, y)) / 4;		// Central pixel
				
		filteredImage._validArea = ImageRect(_validArea.x1 + 1,_validArea.y1 + 1,
											_validArea.x2 - 2, _validArea.y2 - 2);
	}
	return filteredImage;
}


// Position access methods
int FitsPhoto::x() const
{
	return _x;
}


int FitsPhoto::y() const
{
	return _y;
}


void FitsPhoto::x(int x)
{
	_x = x;
}


void FitsPhoto::y(int y)
{
	_y = y;
}


// Valid area access functions.
const ImageRect& FitsPhoto::validArea() const
{
	return _validArea;
}


ImageRect& FitsPhoto::validArea()
{
	return _validArea;
}


// -----------------------------------------------------
ImageRect::ImageRect(int X1, int Y1, int X2, int Y2) :
						x1(X1), y1(Y1), x2(X2), y2(Y2) 
{
}


ImageRect::ImageRect() : x1(0), y1(0), x2(0), y2(0)
{
}


int ImageRect::width() const
{
	return x2 - x1;
}


int ImageRect::height() const
{
	return y2 - y1;
}


void ImageRect::move(int xDispl, int yDispl)
{
	x1 += xDispl;
	x2 += xDispl;
	y1 += yDispl;
	y2 += yDispl;
}


void ImageRect::setRect(int X1, int Y1,int width, int height)
{
	x1 = X1;
	y1 = Y1;
	x2 = X1 + width;
	y2 = Y1 + height;
}


ImageRect ImageRect::intersect(const ImageRect rect) const
{
	if (x2 < rect.x1 || y2 < rect.y1)
		return ImageRect(0, 0, 0, 0);
	
	else if (rect.x2 < x1 || rect.y2 < y2)
		return ImageRect(0, 0, 0, 0);
	
	else if ((x2 > rect.x1 && x2 < rect.x2) && (y2 > rect.y1 && y2 < rect.y2))
		return ImageRect(std::max(x1, rect.x1), std::max(y1, rect.y1), x2, y2);
	
	else //((x1 > rect.x1 && x1 < rect.x2) && (y1 > rect.y1 && y1 < rect.y2))
		return ImageRect(x1, y1, std::min(x2, rect.x2), std::min(y2, rect.y2));
}



namespace
{
	inline ImageRect operationsValidArea(const FitsPhoto& image1, const FitsPhoto& image2)
	{
		ImageRect rect1 = image1.validArea();
		ImageRect rect2 = image2.validArea();
	
		int xDivPos;
		int xDist;
		int yDivPos;
		int yDist;
		int divWidth;
		int divHeight;
		
		if (image1.x() <= image2.x())
		{
			xDivPos = image1.x();
			xDist = image2.x() - image1.x();
			divWidth = std::max<int>(image1.getWidth(), image2.getWidth() + xDist);
		}
		else
		{
			xDivPos = image2.x();
			xDist = image1.x() - image2.x();
			divWidth = std::max<int>(image2.getWidth(), image1.getWidth() + xDist);
		}
		
		if (image1.y() <= image2.y())
		{
			yDivPos = image1.y();
			yDist = image2.y() - image1.y();
			divHeight = std::max<int>(image1.getHeight(), image2.getHeight() + yDist);
		}
		else
		{
			yDivPos = image2.y();
			yDist = image1.y() - image2.y();
			divHeight = std::max<int>(image2.getHeight(), image1.getHeight() + yDist);
		}

		int x_dst = image1.x() - xDivPos;		//x distance of 1st image reference from divImage reference
		int y_dst = image1.y() - yDivPos;		//y distance of 1st image reference from divImage reference
		
		rect1.move(x_dst, y_dst);
		
		x_dst = image2.x() - xDivPos;		//x distance of 2nd image reference from divImage reference
		y_dst = image2.y() - yDivPos;		//y distance of 2nd image reference from divImage reference
		
		rect2.move(x_dst, y_dst);
		
		return rect1.intersect(rect2);
	}
}

