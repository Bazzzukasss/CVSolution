#ifndef APP_CONSTANTS_H
#define APP_CONSTANTS_H

#include <QColor>

namespace app_config {

//settings keys
static const char* AppGuiKey                {"Gui"};
static const char* AppCVKey                 {"CV"};

static const char* GuiWindowGeometryKey     {"windowgeometry"};
static const char* GuiWindowStateKey        {"windowstate"};

static const char* CVMonitorKey             {"cvmonitor"};
static const char* MonitorSourceKey         {"source"};
static const char* MonitorTaskTypeKey       {"task"};
static const char* MonitorTeamKey           {"team"};

//constants
constexpr uint kCVMonitorsCount{4};

static const char* kTaskEye{"Eyes"};
static const char* kTaskLaneMark{"Lane Marks"};

const QColor kRefPenColor{0x0000ff};
const QColor kRefBrushColor{0x1f0000ff};

const QColor kEvalPenColor{0xff0000};
const QColor kEvalBrushColor{0x1fff0000};

const uint kPointRadius{10};

}

#endif // APP_CONSTANTS_H
