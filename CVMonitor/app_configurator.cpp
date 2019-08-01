#include <QDir>
#include <QApplication>
#include <QStandardPaths>
#include "app_configurator.h"

using namespace app_config;

Configurator* Configurator::getInstance()
{
    static AppConfigurator appConfig;
    return &appConfig;
}

Configurator::Configurator(QObject* parent) : QObject(parent)
{}

void Configurator::setConfigurations(const Configurator::ConfigurationsType& configurations)
{
    mConfigurations = configurations;
    for (auto& setting : mConfigurations)
    {
        connect(setting, &CustomSetting::signalDataChanged, this, &Configurator::signalDataChanged);
    }
}

AppConfigurator::AppConfigurator(QObject* parent) : Configurator(parent)
{
    setConfigurations({{SettingsFilename, &mSettings}});
}

void AppConfigurator::loadConfigurations()
{
    for (auto& filename : mConfigurations.keys())
    {
        QSettings settings(getSettingsDirPath() + filename, QSettings::IniFormat);
        mConfigurations[filename]->load(settings);
    }

    emit signalDataLoaded();
}

void AppConfigurator::saveConfigurations() const
{
    for (auto& filename : mConfigurations.keys())
    {
        QSettings settings(getSettingsDirPath() + filename, QSettings::IniFormat);
        mConfigurations[filename]->save(settings);
    }
}

QString AppConfigurator::getSettingsDirPath() const
{
    return QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QDir::separator() + "cvmonitor") + QDir::separator();
}
