#ifndef __QFitsSettingsManager_h__
#define __QFitsSettingsManager_h__

#include <QMap>
#include <QString>
#include <QVariant>
#include <QSettings>

typedef QMap<QString, QVariant> SettingsContainer;

class SettingsManager
{
public:
	SettingsManager(QString settingsFilename);
	QVariant value(QString key);
	void setValue(QString key, QVariant value);
	void beginSection(QString sectionName);
	void endSection();
	void changeSettings(QMap<QString, QVariant> changedSettings);
	void setDef(QString key, QVariant value, QString properties);
	void writeDefaults();


private:
	SettingsContainer _activeSettings;
	SettingsContainer _defaultSettings;
	QMap<QString, QString> _settingsProperties;
	QSettings _settingsFile;
	QString _sectionPrefix;
	
};

#endif //__QFitsSettingsManager_h__