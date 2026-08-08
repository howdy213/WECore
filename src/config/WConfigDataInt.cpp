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