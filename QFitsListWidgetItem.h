#ifndef __QFitsListWidgetItem_h__
#define __QFitsListWidgetItem_h__


#include <QListWidgetItem>
#include "QFitsWindow.h"


class QFitsWindow;

// QFitsListWidgetItem class is a simple QListWidgetItem
// which contains in addition a pointer to a QFitsWindow
// object, necessary to connect each QListWidgetItem to
// its QFitsWindow it represents.


class QFitsListWidgetItem : public QListWidgetItem
{
public:
	QFitsListWidgetItem(QString& imageTitle, QFitsWindow* imageWindowPtr);
	QFitsWindow* getFitsWindowPtr() const;
//	const QFitsWindow* getFitsWindowPtr() const;
	
private:
	QFitsWindow* _fitsWindowPointer;
};

#endif	// __QFitsListWidgetItem_h__