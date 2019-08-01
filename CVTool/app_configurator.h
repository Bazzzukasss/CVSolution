#ifndef APPCONFIGURATOR_H
#define APPCONFIGURATOR_H

#include <QObject>
#include <QMap>
#include "custom_setting.h"
#include "app_settings.h"

#define CONFIG static_cast<app_config::AppConfigurator*>(app_config::AppConfigurator::getInstance())

namespace app_config
{

/**
 * Class Configurator
 * @brief Base singleton class for application settings managing
 */
class Configurator : public QObject
{
    Q_OBJECT
    using ConfigurationsType = QMap<QString, CustomSetting*>;

 public:
    static Configurator* getInstance();

    virtual void loadConfigurations() {}
    virtual void saveConfigurations() const {}

 signals:
    void signalDataChanged();
    void signalDataLoaded();

 protected:
    virtual QString getSettingsDirPath() const { return {}; }

 protected:
    explicit Configurator(QObject* parent = nullptr);
    void setConfigurations(const ConfigurationsType& configurations);

    ConfigurationsType mConfigurations;
};

/**
 * Class AppConfigurator
 * @brief Singleton class derived from Configurator end extended by application settings. responced for
 * saving/loadind/storaging of application settings
 */
class AppConfigurator : public Configurator
{
    Q_OBJECT

    const QString SettingsFilename{"settings.ini"};

 public:
    ApplicationSettings mSettings;


    void loadConfigurations() override;
    void saveConfigurations() const override;

 protected:
    QString getSettingsDirPath() const override;

 private:
    friend class Configurator;
    AppConfigurator(QObject* parent = nullptr);
};
}  // namespace app_config
#endif  // APPCONFIGURATOR_H
