#include "custom_data.h"

app_config::CustomDataInteger::CustomDataInteger(int val, int min, int max, int default_val,
                                                 const QString& value_suffix) :
    CustomDataDigit<int>(val, min, max, default_val),
    suffix(value_suffix)
{}

app_config::CustomDataDouble::CustomDataDouble(double val, double min, double max, double default_val,
                                               int value_decimals, const QString& value_suffix) :
    CustomDataDigit<double>(val, min, max, default_val),
    decimals(value_decimals),
    suffix(value_suffix)
{}
