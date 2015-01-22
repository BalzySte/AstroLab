#include "QFitsSettingsWindow.h"
#include "ui_QFitsSettingsWindow.h"

QFitsSettingsWindow::QFitsSettingsWindow(QWidget *parent) :
    QDialog(parent), ui(new Ui::QFitsSettingsWindow), _app(dynamic_cast<FitsViewer*>(parent))
{
    ui->setupUi(this);
    int settingsListWidth = 1.5 * ui->settingsList->width();
    ui->settingsList->setMaximumWidth(settingsListWidth);
    ui->settingsList->setMaximumWidth(settingsListWidth);

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this,
                      SLOT(buttonBoxInteraction(QAbstractButton*)));
	
    loadSettings();
}


void QFitsSettingsWindow::buttonBoxInteraction(QAbstractButton* pressedButton)
{
    // Manage button pressed with a switch structure
    QDialogButtonBox::StandardButton stdButton = ui->buttonBox->standardButton(pressedButton);
	bool ok;
	
    switch(stdButton)
    {
    case QDialogButtonBox::Ok:
        // Writes out all new settings to settings file
        // Copies "immediate settings" to parent->settings object
        ok = saveSettings();

        if (ok)
		{
            accept();
		}
        else
            // should show an error window.
            break;
        break;

    case QDialogButtonBox::Cancel:
        reject();
        break;

    case QDialogButtonBox::Reset:
        // Reaload parent->settings keys in newSettings
        loadSettings();
        break;

    default:
        break;
    }
}


QFitsSettingsWindow::~QFitsSettingsWindow()
{
	delete ui;
}


bool QFitsSettingsWindow::saveSettings()
{
	_app->settings.setValue("applicationLanguage", ui->languageValue->currentText());
	_app->settings.setValue("nightMode", ui->nightModeValue->isChecked());
    
	// Returns true if settings are fine
	return 1;
}


void QFitsSettingsWindow::loadSettings()
{
	ui->languageValue->setCurrentText(_app->settings.value("applicationLanguage").toString());
	ui->nightModeValue->setChecked(_app->settings.value("nightMode").toBool());
}