#ifndef CUSTOMDATA_H
#define CUSTOMDATA_H

#include <QString>
#include <QStringList>
#include <QMatrix>
#include <QVector>
#include <QMetaType>
#include <limits>

namespace app_config
{

/**
 * Structure CustomData
 * @brief Base struct of custom data types family.
 */
struct CustomData
{};

/**
 * Template structure CustomDataTypeTraits
 * @brief Struct of type traits. Gives possibility to identify CustomDataExt<T> template parameters type.
 */
template <typename T>
struct CustomDataTypeTraits
{
    using ValueType = T;
};

/**
 * Template structure CustomDataExt
 * @brief Base Structure for custom data types family extends by template.
 * Member resetValue can be used to revert value changes by reset() method.
 */
template <typename T>
struct CustomDataExt : public CustomData, public CustomDataTypeTraits<T>
{
    CustomDataExt(T val = {}, T default_val = {});

    T value;
    T defaultValue;
    T resetValue;

    inline void toDefault() { value = defaultValue; }
    inline void reset() { value = resetValue; }
};

/**
 * Template structure CustomDataDigit
 * @brief Base Structure for numerical custom data types family.
 */
template <typename T>
struct CustomDataDigit : public CustomDataExt<T>
{
    CustomDataDigit(T val = 0, T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max(),
                    T default_val = 0);

    T minimum;
    T maximum;
};

/**
 * Template structure CustomDataList
 * @brief Base Structure for list custom data types family. Should be edited via CustomListBox (depends on T type)
 * @param val contains value of one of the item of list
 */
template <typename T>
struct CustomDataList : public CustomDataExt<T>
{
    CustomDataList(const QList<T>& lst = {}, T val = {}, T default_val = {});

    QList<T> list;
};

/**
 * Structure CustomDataInteger
 * @brief Struct represents integer application setting. Should be edited via CustomSpinBox or CustomSlider (or any
 * integer value editing custom widget). Member suffix contains string value of suffix which could be shown in widget.
 */
struct CustomDataInteger : public CustomDataDigit<int>
{
    CustomDataInteger(int val = 0, int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max(),
                      int default_val = std::numeric_limits<int>::min(), const QString& value_suffix = {});

    QString suffix;
};

/**
 * Structure CustomDataDouble
 * @brief Struct represents double application setting. Should be edited via CustomDoubleSpinBox (or any double value
 * editing custom widget). Member suffix contains string value of suffix which could be shown in widget.
 * Member decimals set number of shown digits after dot.
 */
struct CustomDataDouble : public CustomDataDigit<double>
{
    CustomDataDouble(double val = 0, double min = -std::numeric_limits<double>::max(),
                     double max         = std::numeric_limits<double>::max(),
                     double default_val = std::numeric_limits<double>::min(), int value_decimals = 4,
                     const QString& value_suffix = {});

    int decimals;
    QString suffix;
};

template <typename T>
CustomDataExt<T>::CustomDataExt(T val, T default_val) : value(val), defaultValue(default_val), resetValue(val)
{}

template <typename T>
CustomDataDigit<T>::CustomDataDigit(T val, T min, T max, T default_val) :
    CustomDataExt<T>(val, default_val),
    minimum(min),
    maximum(max)
{}

template <typename T>
CustomDataList<T>::CustomDataList(const QList<T>& lst, T val, T default_val) :
    CustomDataExt<T>(val, default_val),
    list(lst)
{}

// To use CustomData family types you can add type alias how shown below or use directly CustomDataExt<T> or
// CustomDataList<T> template classes Moreover there already exists classes CustomDataInteger and CustomDataDouble Dont
// forget add macro Q_DECLARE_METATYPE for your classes
using CustomDataByteArray  = CustomDataExt<QByteArray>;
using CustomDataMatrix     = CustomDataExt<QMatrix>;
using CustomDataUnsigned   = CustomDataExt<unsigned int>;
using CustomDataString     = CustomDataExt<QString>;
using CustomDataBool       = CustomDataExt<bool>;
using CustomDataStringList = CustomDataList<QString>;
using CustomDataCheckList  = CustomDataList<bool>;
using CustomDataIntList    = CustomDataList<int>;
using CustomDataDoubleList = CustomDataList<double>;

}  // namespace app_config

Q_DECLARE_METATYPE(app_config::CustomDataByteArray)
Q_DECLARE_METATYPE(app_config::CustomDataUnsigned)
Q_DECLARE_METATYPE(app_config::CustomDataBool)
Q_DECLARE_METATYPE(app_config::CustomDataInteger)
Q_DECLARE_METATYPE(app_config::CustomDataDouble)
Q_DECLARE_METATYPE(app_config::CustomDataString)
Q_DECLARE_METATYPE(app_config::CustomDataStringList)
Q_DECLARE_METATYPE(app_config::CustomDataCheckList)
Q_DECLARE_METATYPE(app_config::CustomDataIntList)
Q_DECLARE_METATYPE(app_config::CustomDataDoubleList)

#endif  // CUSTOMDATA_H
