#include "WECore/config/WConfigDataString.h"

namespace we::config {

WConfigDataString::WConfigDataString() { m_type = DataType::String; }

WConfigDataBase *WConfigDataString::init(const QString &key,
                                         const WConfigItemInfo &info,
                                         WConfigViewer *parent) {
    WConfigDataBase::init(DataType::String, key, info, parent);
    m_value = info.defaultValue().toString();
    m_original = m_value;
    return this;
}

WConfigDataString *WConfigDataString::init(const QString &key,
                                           const QString &defaultValue,
                                           const Properties &properties,
                                           WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue);
    for (Property p : properties)
        info.property(p);
    init(key, info, parent);
    return this;
}

bool WConfigDataString::setTemporary(const QString &value) {
    if (m_value == value)
        return false;
    m_value = value;
    return true;
}
bool WConfigDataString::setTemporary(const QVariant &value) {
    return setTemporary(value.toString());
}

bool WConfigDataString::setPersistent(const QVariant &val, bool emitSignal) {
    QString newVal = val.toString();
    if (m_original == newVal) return false;
    m_original = newVal;
    if (emitSignal)notifyChange();
    return true;
}

QVariant WConfigDataString::toVariant() const { return m_value; }
bool WConfigDataString::fromVariant(const QVariant &variant) {
    return setTemporary(variant.toString());
}

} // namespace we::config