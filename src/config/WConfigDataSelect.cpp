#include "WECore/config/WConfigDataSelect.h"

namespace we::config {

static QString adjustToOptions(const QString &value,
                               const QStringList &options) {
    if (options.isEmpty())
        return value;
    if (options.contains(value))
        return value;
    return options.first();
}

WConfigDataSelect::WConfigDataSelect() { m_type = DataType::Select; }

WConfigDataBase *WConfigDataSelect::init(const QString &key,
                                         const WConfigItemInfo &info,
                                         WConfigViewer *parent) {
    QString defVal = info.defaultValue().toString();
    QString adjusted = adjustToOptions(defVal, info.options());
    m_value = adjusted;
    m_original = adjusted;
    m_info.defaultValue(adjusted);
    WConfigDataBase::init(DataType::Select, key, info, parent);
    return this;
}
WConfigDataSelect *WConfigDataSelect::init(const QString &key,
                                           const QString &defaultValue,
                                           const QStringList &options,
                                           const Properties &properties,
                                           WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue).defaultItem(defaultValue).options(options);
    for (Property p : properties)
        info.property(p);
    init(key, info, parent);
    return this;
}

bool WConfigDataSelect::setTemporary(const QString &value) {
    if (!options().contains(value) || m_value == value)
        return false;
    m_value = value;
    return true;
}
bool WConfigDataSelect::setTemporary(const QVariant &value) {
    return setTemporary(value.toString());
}

bool WConfigDataSelect::setPersistent(const QVariant &val, bool emitSignal) {
    QString newVal = val.toString();
    QString adjusted = adjustToOptions(newVal, m_info.options());
    if (!options().contains(adjusted) || m_original == adjusted)
        return false;
    m_original = adjusted;
    if (emitSignal)
        notifyChange();
    return true;
}

QVariant WConfigDataSelect::toVariant() const { return m_value; }
bool WConfigDataSelect::fromVariant(const QVariant &variant) {
    return setTemporary(variant.toString());
}

void WConfigDataSelect::addOption(const QString &option) {
    if (!options().contains(option))
        m_info.addOption(option);
}
void WConfigDataSelect::addOptions(const QStringList &options) {
    for (const QString &o : options)
        addOption(o);
}

} // namespace we::config