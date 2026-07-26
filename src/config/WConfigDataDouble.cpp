#include "WECore/config/WConfigDataDouble.h"
#include <cmath>

namespace we::config {

WConfigDataDouble::WConfigDataDouble() { m_type = DataType::Double; }

WConfigDataBase *WConfigDataDouble::init(const QString &key,
                                         const WConfigItemInfo &info,
                                         WConfigViewer *parent) {
    WConfigDataBase::init(DataType::Double, key, info, parent);
    m_value = info.defaultValue().toDouble();
    m_original = m_value;
    return this;
}

WConfigDataDouble *WConfigDataDouble::init(const QString &key,
                                           double defaultValue,
                                           int decimalPlaces,
                                           const Properties &properties,
                                           WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue).decimalPlaces(decimalPlaces);
    for (Property p : properties)
        info.property(p);
    init(key, info, parent);
    return this;
}

bool WConfigDataDouble::setTemporary(double value) {
    int places = m_info.decimalPlaces();
    double factor = std::pow(10.0, places);
    double rounded = std::round(value * factor) / factor;
    if (qFuzzyCompare(m_value, rounded))
        return false;
    m_value = rounded;
    return true;
}
bool WConfigDataDouble::setTemporary(const QVariant &value) {
    return setTemporary(value.toDouble());
}

bool WConfigDataDouble::setPersistent(const QVariant &val, bool emitSignal) {
    double newVal = val.toDouble();
    if (qFuzzyCompare(m_original, newVal)) return false;
    m_original = newVal;
    if (emitSignal)notifyChange();
    return true;
}

QVariant WConfigDataDouble::toVariant() const { return m_value; }
bool WConfigDataDouble::fromVariant(const QVariant &variant) {
    return setTemporary(variant.toDouble());
}

} // namespace we::config