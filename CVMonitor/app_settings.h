#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include "custom_setting.h"
#include "app_constants.h"

namespace app_config
{

class MonitorSettings : public CustomSetting
{
 public:
    MonitorSettings(const QString& key, QObject* parent = nullptr);
    CustomSettingExt<CustomDataString> mSource{MonitorSourceKey};
    CustomSettingExt<CustomDataStringList> mTaskType{ MonitorTaskTypeKey, {{kTaskEye, kTaskLaneMark}, kTaskEye, kTaskEye}};
    CustomSettingExt<CustomDataInteger> mTeam{MonitorTeamKey,{0,0,100,0}};
};

class ApplicationSettings : public CustomSetting
{

    class CVSettings : public CustomSetting
    {
        public:
        CVSettings(const QString& key, QObject* parent = nullptr);

        QVector<CustomSetting*> mMonitorSettings;
    };

    class GuiSettings : public CustomSetting
    {
     public:
        GuiSettings(const QString& key, QObject* parent = nullptr);
        CustomSettingExt<CustomDataByteArray> mWindowGeometry{GuiWindowGeometryKey};
        CustomSettingExt<CustomDataByteArray> mWindowState{GuiWindowStateKey};
    };

public:
    ApplicationSettings(QObject* parent = nullptr);

    GuiSettings mGui{AppGuiKey};
    CVSettings mCV{AppCVKey};
};


}  // namespace app_config

#endif  // APPSETTINGS_H
