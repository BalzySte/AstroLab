#include "QTextInfoWindow.h"


QTextInfoWindow::QTextInfoWindow(const char* titleString)
{	
	setAttribute(Qt::WA_DeleteOnClose, true);
	
	_layout = new QGridLayout();
	
	_buttonBox = new QDialogButtonBox(this);
	_buttonBox->addButton("Ok", QDialogButtonBox::AcceptRole);
	
	// Connects Ok button pressure event to windows destruction
	connect(_buttonBox, SIGNAL(accepted()), this, SLOT(OkButtonPressed()));
	
	_textWidget = new QTextEdit();
	_textWidget->setReadOnly(true);
	_textWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		
	_layout->addWidget(_textWidget, 0, 0, 1, 1);
	_layout->addWidget(_buttonBox, 1, 0, 1, 1, Qt::AlignHCenter);
	
	this->setLayout(_layout);
		
 	setWindowTitle(titleString);
	resize(800, 400);
	
	show();
}


QTextInfoWindow::~QTextInfoWindow()
{
	// Since Class inherits QWidget, it's virtual destructor
	// will be called as this functions terminates, deleting all
	// children. No memory leak should occur.
}


void QTextInfoWindow::setText(std::string text)
{
	_textWidget->setText(QString::fromStdString(text));
}


void QTextInfoWindow::OkButtonPressed()
{
	this->close();
}