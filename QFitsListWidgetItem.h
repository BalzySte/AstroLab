#ifndef __QFitsListWidgetItem_h__
#define __QFitsListWidgetItem_h__


#include <QtGui>
#include "QFitsWindow.h"
#include "settings.h"

#include "common.h"


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