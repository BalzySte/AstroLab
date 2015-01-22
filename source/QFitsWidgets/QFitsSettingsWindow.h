#ifndef __QFitsSettingsWindow_h__
#define __QFitsSettingsWindow_h__

#include <QDialog>
#include <QSettings>
#include <QDialogButtonBox>
#include "fitsviewer.h"

//TODO: In future this class should be able to display settings
//      automatically, parsing application settings elements

namespace Ui
{
	class QFitsSettingsWindow;
}

class QFitsSettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit QFitsSettingsWindow(QWidget *parent = 0);
    ~QFitsSettingsWindow();

public slots:
    void buttonBoxInteraction(QAbstractButton* pressedButton);
private:
    Ui::QFitsSettingsWindow *ui;

    bool saveSettings();	// Reads settings in ui widgets
							// and save them

    void loadSettings();	// Reads application settings
							// and shows them in ui widgets
	
	FitsViewer* _app;
};

#endif //__QFitsSettingsWindow_h__

