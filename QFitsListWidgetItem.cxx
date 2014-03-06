#include "QFitsListWidgetItem.h"


QFitsListWidgetItem::QFitsListWidgetItem(QString& imageTitle,
			QFitsWindow* imageWindowPtr) : QListWidgetItem(imageTitle)
{
	_fitsWindowPointer = imageWindowPtr;
}


QFitsWindow* QFitsListWidgetItem::getFitsWindowPtr() const
{
	return _fitsWindowPointer;
}

/*
const QFitsWindow* QFitsListWidgetItem::getFitsWindowPtr() const
{
	return const_cast<const QFitsWindow*>(_fitsWindowPointer);	
}
*/