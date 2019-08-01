#ifndef CUSTOMWIDGETS_H
#define CUSTOMWIDGETS_H

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QProxyStyle>
#include "custom_setting.h"

/**
 * Macros for generation custom widgets classes
 * @brief Custom widgets classes coud be binded to CustomSetting to edit it by user.
 * @param type define name of generated class
 * @param parent define parent clas of generated class
 * @param setting define type of CustomSetting family class whicj will be binded to generated widget
 */

#define CUSTOM_WIDGET(type, parent, setting) \
    class type : public parent \
    { \
        Q_OBJECT \
        using SettingType = app_config::setting; \
\
     public: \
        explicit type(QWidget* parent = nullptr); \
        void bindToSetting(SettingType* setting); \
\
     private: \
        void update(); \
        SettingType* mSetting; \
    };

// Dont forget to add an imlementation of generated classes
CUSTOM_WIDGET(CustomSpinBox, QSpinBox, SettingInt)
CUSTOM_WIDGET(CustomSlider, QSlider, SettingInt)
CUSTOM_WIDGET(CustomDoubleSpinBox, QDoubleSpinBox, SettingDouble)
CUSTOM_WIDGET(CustomCheckBox, QCheckBox, SettingBool)
CUSTOM_WIDGET(CustomLineEdit, QLineEdit, SettingString)
CUSTOM_WIDGET(CustomComboBox, QComboBox, SettingStringList)

/**
 * Class SeekBarStyle
 * @brief Class to correct QSlider behave
 */
class SeekBarStyle : public QProxyStyle
{
 public:
    using QProxyStyle::QProxyStyle;

    int styleHint(QStyle::StyleHint hint, const QStyleOption* option = 0, const QWidget* widget = 0,
                  QStyleHintReturn* returnData = 0) const
    {
        if (hint == QStyle::SH_Slider_AbsoluteSetButtons)
        {
            return (Qt::LeftButton | Qt::MidButton | Qt::RightButton);
        }
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

#endif  // CUSTOMWIDGETS_H
