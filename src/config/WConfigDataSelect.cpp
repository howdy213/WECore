/**
 * @author howdy213
 * @date 2026-08-08
 * @version 2.0.0
 *
 * Copyright 2025-2026 howdy213
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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