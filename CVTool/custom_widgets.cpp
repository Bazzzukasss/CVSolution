#include "custom_widgets.h"

using namespace app_config;

CustomSpinBox::CustomSpinBox(QWidget* parent) : QSpinBox(parent), mSetting(nullptr)
{}

void CustomSpinBox::bindToSetting(CustomSpinBox::SettingType* setting)
{
    if (setting != nullptr)
    {
        mSetting = setting;
        connect(this, &CustomSpinBox::editingFinished, this, [&]() { mSetting->setDataValue(value()); });
        connect(mSetting, &CustomSetting::signalDataChanged, this, &CustomSpinBox::update);
        update();
    }
}

void CustomSpinBox::update()
{
    if (mSetting != nullptr)
    {
        setMinimum(mSetting->getData().minimum);
        setMaximum(mSetting->getData().maximum);
        setSuffix(mSetting->getData().suffix);
        setValue(mSetting->getData().value);
    }
}

CustomSlider::CustomSlider(QWidget* parent) : QSlider(parent), mSetting(nullptr)
{}

void CustomSlider::bindToSetting(CustomSlider::SettingType* setting)
{
    if (setting != nullptr)
    {
        mSetting = setting;
        connect(this, &CustomSlider::valueChanged, this, [&]() { mSetting->setDataValue(value()); });
        connect(mSetting, &CustomSetting::signalDataChanged, this, &CustomSlider::update);
        update();
    }
}

void CustomSlider::update()
{
    if (mSetting != nullptr)
    {
        setMinimum(mSetting->getData().minimum);
        setMaximum(mSetting->getData().maximum);
        setValue(mSetting->getData().value);
    }
}

CustomDoubleSpinBox::CustomDoubleSpinBox(QWidget* parent) : QDoubleSpinBox(parent), mSetting(nullptr)
{}

void CustomDoubleSpinBox::bindToSetting(CustomDoubleSpinBox::SettingType* setting)
{
    if (setting != nullptr)
    {
        mSetting = setting;
        connect(this, &CustomDoubleSpinBox::editingFinished, this, [&]() { mSetting->setDataValue(value()); });
        connect(mSetting, &CustomSetting::signalDataChanged, this, &CustomDoubleSpinBox::update);
        update();
    }
}

void CustomDoubleSpinBox::update()
{
    if (mSetting != nullptr)
    {
        setMinimum(mSetting->getData().minimum);
        setMaximum(mSetting->getData().maximum);
        setDecimals(mSetting->getData().decimals);
        setSuffix(mSetting->getData().suffix);
        setValue(mSetting->getData().value);
    }
}

CustomCheckBox::CustomCheckBox(QWidget* parent) : QCheckBox(parent), mSetting(nullptr)
{}

void CustomCheckBox::bindToSetting(CustomCheckBox::SettingType* setting)
{
    if (setting != nullptr)
    {
        mSetting = setting;
        connect(this, &CustomCheckBox::stateChanged, this, [&]() { mSetting->setDataValue(isChecked()); });
        connect(mSetting, &CustomSetting::signalDataChanged, this, &CustomCheckBox::update);
        update();
    }
}

void CustomCheckBox::update()
{
    if (mSetting != nullptr)
    {
        blockSignals(true);
        setChecked(mSetting->getData().value);
        blockSignals(false);
    }
}

CustomLineEdit::CustomLineEdit(QWidget* parent) : QLineEdit(parent), mSetting(nullptr)
{}

void CustomLineEdit::bindToSetting(CustomLineEdit::SettingType* setting)
{
    if (setting != nullptr)
    {
        mSetting = setting;
        connect(this, &CustomLineEdit::textChanged, this, [&]() { mSetting->setDataValue(text()); });
        connect(this, &CustomLineEdit::editingFinished, this, [&]() { mSetting->setDataValue(text()); });
        connect(mSetting, &CustomSetting::signalDataChanged, this, &CustomLineEdit::update);
        update();
    }
}

void CustomLineEdit::update()
{
    if (mSetting != nullptr)
    {
        blockSignals(true);
        setText(mSetting->getData().value);
        blockSignals(false);
    }
}

CustomComboBox::CustomComboBox(QWidget* parent) : QComboBox(parent), mSetting(nullptr)
{}

void CustomComboBox::bindToSetting(CustomComboBox::SettingType* setting)
{
    if (setting != nullptr)
    {
        mSetting = setting;
        connect(this, &CustomComboBox::currentTextChanged, this, [&]() { mSetting->setDataValue(currentText()); });
        connect(mSetting, &CustomSetting::signalDataChanged, this, &CustomComboBox::update);
        update();
    }
}

void CustomComboBox::update()
{
    if (mSetting != nullptr)
    {
        blockSignals(true);
        clear();
        for (auto item : mSetting->getData().list)
        {
            addItem(item);
        }
        setCurrentText(mSetting->getData().value);
        blockSignals(false);
    }
}
