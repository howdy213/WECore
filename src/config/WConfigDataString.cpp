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