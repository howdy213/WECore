#include "WECore/config/WConfigDataBool.h"

namespace we::config {

WConfigDataBool::WConfigDataBool() { m_type = DataType::Bool; }

WConfigDataBase *WConfigDataBool::init(const QString &key,
                                       const WConfigItemInfo &info,
                                       WConfigViewer *parent) {
    WConfigDataBase::init(DataType::Bool, key, info, parent);
    m_value = info.defaultValue().toBool();
    m_original = m_value;
    return this;
}

WConfigDataBool *WConfigDataBool::init(const QString &key, bool defaultValue,
                                       const Properties &properties,
                                       WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue);
    for (Property p : properties)
        info.property(p);
    init(key, info, parent);
    return this;
}

bool WConfigDataBool::setTemporary(bool value) {
    if (m_value == value)
        return false;
    m_value = value;
    return true;
}
bool WConfigDataBool::setTemporary(const QVariant &value) {
    return setTemporary(value.toBool());
}

bool WConfigDataBool::setPersistent(const QVariant &val, bool emitSignal) {
    bool newVal = val.toBool();
    if (m_original == newVal)
        return false;
    m_original = newVal;
    if (emitSignal)
        notifyChange();
    return true;
}

bool WConfigDataBool::setPersistent(bool val) {
    bool newVal = val;
    if (m_original == newVal)
        return false;
    m_original = newVal;
    notifyChange();
    return true;
}

QVariant WConfigDataBool::toVariant() const { return m_value; }
bool WConfigDataBool::fromVariant(const QVariant &variant) {
    return setTemporary(variant.toBool());
}

} // namespace we::config