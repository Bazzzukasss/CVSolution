#ifndef CUSTOMSETTING_H
#define CUSTOMSETTING_H

#include <QObject>
#include <QVector>
#include <QSettings>
#include "custom_data.h"

namespace app_config
{

/**
 * Class CustomSetting
 * @brief General base class for any kind of application settings subclasses. Provides unific interface for
 * AppConfigurator routines.
 */
class CustomSetting : public QObject
{
    Q_OBJECT
 public:
    CustomSetting(const QString& key, QObject* parent = nullptr);
    virtual ~CustomSetting() = default;

    void addSettings(QVector<CustomSetting*> settings);

    virtual void load(QSettings& settings, const QString& parentKey = {});
    virtual void save(QSettings& settings, const QString& parentKey = {});

 signals:
    void signalDataChanged();

 protected:
    inline void emitSignalDataChanged() { emit signalDataChanged(); }

    virtual void setValue(const QVariant&) {}
    virtual QVariant getValue() const { return {}; }
    virtual QVariant getDefaultValue() const { return {}; }

    QVector<CustomSetting*> mCustomSettings;
    QString mKey;
};

/**
 * Tepmlate class CustomSettingExt<T>
 * @brief Base temlate class for application settings. Provides unific interface for manipulation of setting data (T
 * Data) via CustomWidgets family classes
 */
template <typename T>
class CustomSettingExt : public CustomSetting
{
    using DataValueType = typename T::ValueType;

 public:
    CustomSettingExt(const QString& key, const T& data = {}, QObject* parent = nullptr);

    void setData(const T& data) { mData = data; }
    T& getData() { return mData; }

    DataValueType getDataValue() const { return mData.value; }
    DataValueType getDataDefaultValue() const { return mData.defaultValue; }
    void setDataValue(DataValueType value);

    operator DataValueType() const;
    CustomSettingExt<T>& operator=(const DataValueType& value);

 protected:
    void setValue(const QVariant& variant) override { setDataValue(variant.value<DataValueType>()); }
    QVariant getValue() const override { return getDataValue(); }
    QVariant getDefaultValue() const override { return getDataDefaultValue(); }

 private:
    T mData;
};

template <typename T>
CustomSettingExt<T>::CustomSettingExt(const QString& key, const T& data, QObject* parent) :
    CustomSetting(key, parent),
    mData(data)
{}

template <typename T>
void CustomSettingExt<T>::setDataValue(CustomSettingExt<T>::DataValueType value)
{
    if (mData.value != value)
    {
        mData.value = value;
        emitSignalDataChanged();
    }
}

template <typename T>
CustomSettingExt<T>& CustomSettingExt<T>::operator=(const CustomSettingExt::DataValueType& value)
{
    setDataValue(value);
    return *this;
}

template <typename T>
CustomSettingExt<T>::operator DataValueType() const
{
    return getDataValue();
}

/**
 * Template class CustomSettingArray<T>
 * @brief Class represents array of CustomData objects
 */
template <typename T>
class CustomSettingArray : public CustomSetting
{
    using DataValueType = typename T::ValueType;

 public:
    CustomSettingArray(const QString& key, const QVector<T>& data = {}, QObject* parent = nullptr);

    inline void addSettings(QVector<CustomSetting*> settings) = delete;

    void load(QSettings& settings, const QString& parentKey) override;
    void save(QSettings& settings, const QString& parentKey) override;

    void setData(const QVector<T>& data) { mData = data; }
    QVector<T>& getData() { return mData; }
    void clearData();

    DataValueType getDataValue() const;
    DataValueType getDataDefaultValue() const;
    void setDataValue(DataValueType value);
    void addDataValue(DataValueType value);

    void setArrayIndex(int index);

    operator DataValueType() const;

    CustomSettingArray<T>& operator=(const DataValueType& value);
    CustomSettingArray<T>& operator=(const QVector<T>& data);

 protected:
    void setValue(const QVariant& variant) override { setDataValue(variant.value<DataValueType>()); }
    QVariant getValue() const override { return QVariant::fromValue(getDataValue()); }
    QVariant getDefaultValue() const override { return QVariant::fromValue(getDataDefaultValue()); }

 private:
    int mArrayIndex;
    QVector<T> mData;
};

template <typename T>
CustomSettingArray<T>::CustomSettingArray(const QString& key, const QVector<T>& data, QObject* parent) :
    CustomSetting(key, parent),
    mArrayIndex(-1),
    mData(data)
{}

template <typename T>
void CustomSettingArray<T>::load(QSettings& settings, const QString& parentKey)
{
    const auto& key = parentKey + "/" + mKey;

    int size = settings.beginReadArray(key);
    for (int i = 0; i < size; ++i)
    {
        if (i >= mData.size())
            mData.push_back({});

        setArrayIndex(i);
        settings.setArrayIndex(i);
        auto value = settings.value("value", getDefaultValue());
        if (value.isValid())
        {
            setValue(value);
        }
    }
    settings.endArray();
}

template <typename T>
void CustomSettingArray<T>::save(QSettings& settings, const QString& parentKey)
{
    const auto& key = parentKey + "/" + mKey;

    settings.beginWriteArray(key);
    for (int i = 0; i < mData.size(); ++i)
    {
        setArrayIndex(i);
        settings.setArrayIndex(i);

        const auto& value = getValue();
        if (value.isValid())
        {
            settings.setValue("value", value);
        }
    }
    settings.endArray();
}

template <typename T>
typename CustomSettingArray<T>::DataValueType CustomSettingArray<T>::getDataValue() const
{
    return mArrayIndex >= 0 ? mData[mArrayIndex].value : DataValueType();
}

template <typename T>
typename CustomSettingArray<T>::DataValueType CustomSettingArray<T>::getDataDefaultValue() const
{
    return mArrayIndex >= 0 ? mData[mArrayIndex].defaultValue : DataValueType();
}

template <typename T>
void CustomSettingArray<T>::setDataValue(CustomSettingArray::DataValueType value)
{
    if (mArrayIndex >= 0)
    {
        if (mData[mArrayIndex].value != value)
        {
            mData[mArrayIndex].value = value;
            emitSignalDataChanged();
        }
    }
}

template <typename T>
void CustomSettingArray<T>::addDataValue(CustomSettingArray::DataValueType value)
{
    mData.push_back(value);
}

template <typename T>
void CustomSettingArray<T>::setArrayIndex(int index)
{
    if (index < mData.size() && index >= 0)
        mArrayIndex = index;
}

template <typename T>
CustomSettingArray<T>& CustomSettingArray<T>::operator=(const CustomSettingArray::DataValueType& value)
{
    setDataValue(value);
    return *this;
}

template <typename T>
CustomSettingArray<T>& CustomSettingArray<T>::operator=(const QVector<T>& data)
{
    setData(data);
    return *this;
}

template <typename T>
void CustomSettingArray<T>::clearData()
{
    mData.clear();
}

template <typename T>
CustomSettingArray<T>::operator DataValueType() const
{
    return getDataValue();
}

using SettingArrayString = CustomSettingArray<CustomDataString>;

using SettingInt        = CustomSettingExt<CustomDataInteger>;
using SettingDouble     = CustomSettingExt<CustomDataDouble>;
using SettingBool       = CustomSettingExt<CustomDataBool>;
using SettingString     = CustomSettingExt<CustomDataString>;
using SettingStringList = CustomSettingExt<CustomDataStringList>;
using SettingCheckList  = CustomSettingExt<CustomDataCheckList>;
}  // namespace app_config

#endif  // CUSTOMSETTING_H
