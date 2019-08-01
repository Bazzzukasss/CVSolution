#include <QDataStream>
#include "app_settings.h"
#include "app_constants.h"

using namespace app_config;

ApplicationSettings::GuiSettings::GuiSettings(const QString& key, QObject* parent) : CustomSetting(key, parent)
{
    addSettings({&mWindowGeometry, &mWindowState});
}

ApplicationSettings::ApplicationSettings(QObject* parent) : CustomSetting({}, parent)
{
    addSettings({&mGui,&mCV});
}

ApplicationSettings::CVSettings::CVSettings(const QString &key, QObject *parent) : CustomSetting(key, parent)
{
    for(int i = 0; i < app_config::kCVMonitorsCount; ++i)
    {
        mMonitorSettings.push_back( new MonitorSettings(app_config::CVMonitorKey + QString::number(i), this) );
    }

    addSettings(mMonitorSettings);
}

MonitorSettings::MonitorSettings(const QString &key, QObject *parent) : CustomSetting(key, parent)
{
    addSettings({&mSource, &mTaskType, &mTeam});
}
