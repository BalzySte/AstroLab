#ifndef __QMousePointerDock_h__
#define __QMousePointerDock_h__

#include <QtGui>

#include <sstream>

class QMousePointerDock : public QDockWidget
{
public:
	QMousePointerDock(QWidget* parent);
	void update();

private:
	QLabel *_textLabel;
	std::stringstream _textStream;
};


#endif	// __QMousePointerDock_h__