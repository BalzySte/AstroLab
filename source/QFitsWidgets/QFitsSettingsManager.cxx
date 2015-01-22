#include "QFitsSettingsManager.h"


SettingsManager::SettingsManager(QString settingsFilename) : _settingsFile(settingsFilename, QSettings::IniFormat), _sectionPrefix("")
{
}


void SettingsManager::writeDefaults()
{
	QStringList allKeys = _settingsFile.allKeys();
	
	QStringList::const_iterator it = allKeys.constBegin();
	
	while (it != allKeys.constEnd())
	{
		_activeSettings.insert(*it, _settingsFile.value(*it));
		++it;
	}
	
	SettingsContainer::const_iterator it2 = _defaultSettings.constBegin();
	QVariant invalid_variant = QVariant();
	
	while (it2 != _defaultSettings.constEnd())
	{
		if (_activeSettings.value(it2.key()) == invalid_variant) 
			setValue(it2.key(), it2.value());
		
		++it2;
	}
}


QVariant SettingsManager::value(QString key)
{
	return _activeSettings.value(key);
}


void SettingsManager::setValue(QString key, QVariant value)
{
	_settingsFile.setValue(key, value);
	
	if (!(_settingsProperties.value(key).contains("restart")))
		_activeSettings.insert(key, value);
}


void SettingsManager::setDef(QString key, QVariant value, QString properties)
{
	_defaultSettings.insert(key, value);
	_settingsProperties.insert(key, properties);
}


void SettingsManager::beginSection(QString sectionName)
{
	_sectionPrefix += sectionName;
	_sectionPrefix += "/";
}


void SettingsManager::endSection()
{
	_sectionPrefix.truncate(_sectionPrefix.lastIndexOf("/", -2) + 1);
}


void SettingsManager::changeSettings(SettingsContainer newSettings)
{
	SettingsContainer::const_iterator it = newSettings.constBegin();
	
	while (it != newSettings.constEnd())
	{
		setValue(it.key(), it.value());
		++it;
	}
}