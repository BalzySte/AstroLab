#include <QtGui>


class QTextInfoWindow : public QWidget
{
	Q_OBJECT

public:
	QTextInfoWindow(const char* titleString = "");
	virtual ~QTextInfoWindow();
	
	void setText(QString text);
	void setText(std::string text);
	void setTitle(char* title);

private:
//	QWidget* _container;	// Main widget containing QGridLayout
	QGridLayout* _layout;	// Layout containing all Dock widgets
	
	QTextEdit* _textWidget;
	QDialogButtonBox* _buttonBox;
	
private slots:
	void OkButtonPressed();
};
