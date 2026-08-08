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
#include "WECore/config/WConfigDataArray.h"
#include <QDebug>

namespace we::config {

WConfigDataArray::WConfigDataArray() { m_type = DataType::Array; }

WConfigDataBase *WConfigDataArray::init(const QString &key,
                                        const WConfigItemInfo &info,
                                        WConfigViewer *parent) {
    WConfigDataBase::init(DataType::Array, key, info, parent);
    if (info.defaultValue().canConvert<QVariantList>()) {
        auto list = m_info.defaultValue().toList();
        for (auto &v : list) {
            v = convertVariantToType(v, elementType());
        }
        m_info.defaultValue(list);
        m_value = list;
        m_originalList = m_value;
    } else {
        QVariant defaultValue = defaultVariantForType(info.elementType());
        m_info.defaultValue(QVariantList{defaultValue});
    }
    return this;
}

WConfigDataArray *WConfigDataArray::init(const QString &key,
                                         const QVariantList &defaultValue,
                                         DataType elementType,
                                         const Properties &properties,
                                         WConfigViewer *parent) {
    WConfigItemInfo info;
    info.elementType(elementType).defaultValue(defaultValue);
    for (Property p : properties)
        info.property(p);
    init(key, info, parent);
    m_value = defaultValue;
    return this;
}

bool WConfigDataArray::setTemporary(const QVariant &value) {
    if (!value.canConvert<QVariantList>())
        return false;
    QVariantList newList = value.toList();
    for (auto &v : newList) {
        v = convertVariantToType(v, elementType());
    }
    if (m_value == newList)
        return false;
    m_value = newList;
    return true;
}

bool WConfigDataArray::setPersistent(const QVariant &val, bool emitSignal) {
    if (!val.canConvert<QVariantList>())
        return false;
    bool changed = m_originalList != val.toList();
    if (changed) {
        m_originalList = val.toList();
        if (emitSignal)
            notifyChange();
    }
    return changed;
}

bool WConfigDataArray::fromVariant(const QVariant &variant) {
    return setTemporary(variant);
}

QVariant WConfigDataArray::elementAt(int index) const {
    return (index >= 0 && index < m_value.count()) ? m_value[index] : QVariant();
}

bool WConfigDataArray::addElement(const QVariant &element) {
    QVariant converted = convertVariantToType(element, info().elementType());
    m_value.append(converted);
    return true;
}

bool WConfigDataArray::setElement(int index, const QVariant &element) {
    if (index < 0 || index >= m_value.count())
        return false;
    m_value[index] = element;
    return true;
}

bool WConfigDataArray::insertElement(int index, const QVariant &element) {
    if (index < 0 || index > m_value.count())
        return false;
    QVariant converted = convertVariantToType(element, m_info.elementType());
    m_value.insert(index, converted);
    return true;
}

bool WConfigDataArray::removeElement(int index) {
    if (index < 0 || index >= m_value.count())
        return false;
    m_value.removeAt(index);
    return true;
}

} // namespace we::config