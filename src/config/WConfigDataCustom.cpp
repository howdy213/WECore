/**
 * @author howdy213
 * @date 2026-09-25
 * @version 2.1.0
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
#include "WECore/config/WConfigDataCustom.h"
#include "WECore/config/WConfigCustomType.h"

namespace we::config {

namespace {
const WConfigCustomType *typeFor(const QString &typeName) {
    if (typeName.isEmpty())
        return nullptr;
    return WConfigCustomTypeRegistry::instance().type(typeName);
}

bool valuesEqual(const QVariant &a, const QVariant &b,
                 const WConfigCustomType *t) {
    if (t && t->equal)
        return t->equal(a, b);
    return a == b;
}
} // namespace

WConfigDataCustom::WConfigDataCustom() { m_type = DataType::Custom; }

WConfigDataBase *WConfigDataCustom::init(const QString &key,
                                         const WConfigItemInfo &info,
                                         WConfigViewer *parent) {
    WConfigDataBase::init(DataType::Custom, key, info, parent);
    return this;
}

WConfigDataCustom *WConfigDataCustom::init(const QString &key,
                                           const QString &typeName,
                                           const QVariant &defaultValue,
                                           const Properties &properties,
                                           WConfigViewer *parent) {
    WConfigItemInfo info;
    info.displayName(key).elementType(DataType::Custom);
    for (Property p : properties)
        info.property(p);
    WConfigDataBase::init(DataType::Custom, key, info, parent);
    m_typeName = typeName;
    m_value = defaultValue;
    m_saved = toVariant(); // Store the serialized default value
    return this;
}

bool WConfigDataCustom::setTemporary(const QVariant &value) {
    if (valuesEqual(m_value, value, typeFor(m_typeName)))
        return false;
    m_value = value;
    notifyChange();
    return true;
}

bool WConfigDataCustom::setPersistent(const QVariant &val, bool emitSignal) {
    Q_UNUSED(emitSignal);
    m_saved = val;
    if (const WConfigCustomType *t = typeFor(m_typeName))
        m_value = t->deserialize(val);
    else
        m_value = val;
    return true;
}

bool WConfigDataCustom::modified() const {
    return !valuesEqual(toVariant(), m_saved, typeFor(m_typeName));
}

QVariant WConfigDataCustom::toVariant() const {
    if (const WConfigCustomType *t = typeFor(m_typeName))
        return t->serialize(m_value);
    return m_value;
}

bool WConfigDataCustom::fromVariant(const QVariant &variant) {
    m_saved = variant;
    if (const WConfigCustomType *t = typeFor(m_typeName))
        m_value = t->deserialize(variant);
    else
        m_value = variant;
    return true;
}

} // namespace we::config