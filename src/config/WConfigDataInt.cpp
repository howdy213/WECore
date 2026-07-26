#include "WECore/config/WConfigDataInt.h"

namespace we::config {

WConfigDataInt::WConfigDataInt() { m_type = DataType::Int; }

WConfigDataBase *WConfigDataInt::init(const QString &key,
                                      const WConfigItemInfo &info,
                                      WConfigViewer *parent) {
    WConfigDataBase::init(DataType::Int, key, info, parent);
    m_value = info.defaultValue().toInt();
    m_original = m_value;
    return this;
}

WConfigDataInt *WConfigDataInt::init(const QString &key, int defaultValue,
                                     const Properties &properties,
                                     WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue);
    for (Property p : properties)
        info.property(p);
    init(key, info, parent);
    return this;
}

bool WConfigDataInt::setTemporary(int value) {
    if (m_value == value)
        return false;
    m_value = value;
    return true;
}

bool WConfigDataInt::setTemporary(const QVariant &value) {
    return setTemporary(value.toInt());
}

bool WConfigDataInt::setPersistent(const QVariant &val, bool emitSignal) {
    int newVal = val.toInt();
    if (m_original == newVal) return false;
    m_original = newVal;
    if (emitSignal)notifyChange();
    return true;
}

QVariant WConfigDataInt::toVariant() const { return m_value; }
bool WConfigDataInt::fromVariant(const QVariant &variant) {
    return setTemporary(variant.toInt());
}

} // namespace we::config