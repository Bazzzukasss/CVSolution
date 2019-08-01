#include "custom_setting.h"

using namespace app_config;

CustomSetting::CustomSetting(const QString& key, QObject* parent) : QObject(parent), mKey(key)
{}

void CustomSetting::addSettings(QVector<CustomSetting*> settings)
{
    mCustomSettings.append(settings);
    for (auto& setting : settings)
    {
        connect(setting, &CustomSetting::signalDataChanged, this, &CustomSetting::signalDataChanged);
    }
}

void CustomSetting::load(QSettings& settings, const QString& parentKey)
{
    const auto& key = parentKey + "/" + mKey;
    auto value      = settings.value(key, getDefaultValue());
    if (value.isValid())
    {
        setValue(value);
    }

    for (auto& customSetting : mCustomSettings)
    {
        customSetting->load(settings, key);
    }
}

void CustomSetting::save(QSettings& settings, const QString& parentKey)
{
    const auto& key   = parentKey + "/" + mKey;
    const auto& value = getValue();
    if (value.isValid())
    {
        settings.setValue(key, value);
    }

    for (auto& customSetting : mCustomSettings)
    {
        customSetting->save(settings, key);
    }
}
