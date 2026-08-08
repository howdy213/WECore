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
#ifndef WCONFIGITEMINFO_H
#define WCONFIGITEMINFO_H

#include "WConfigDef.h"
#include <QString>
#include <QVariant>

namespace we::config {

class WE_EXPORT WConfigItemInfo {
public:
    WConfigItemInfo() = default;

    // 链式设置
    WConfigItemInfo &displayName(const QString &name) {
        m_displayName = name;
        return *this;
    }
    WConfigItemInfo &description(const QString &desc) {
        m_description = desc;
        return *this;
    }
    WConfigItemInfo &elementType(DataType t) {
        m_elementType = t;
        return *this;
    }
    WConfigItemInfo &property(Property p) {
        m_properties.append(p);
        return *this;
    }
    WConfigItemInfo &decimalPlaces(int places) {
        m_decimalPlaces = places;
        return *this;
    }
    WConfigItemInfo &defaultItem(const QString &item) {
        m_defaultItem = item;
        return *this;
    }
    WConfigItemInfo &defaultValue(const QVariant &val) {
        if (val.typeId() == QMetaType::LongLong){
            m_defaultValue = val.toInt();
            return *this;
        }
        m_defaultValue = val;
        return *this;
    }
    WConfigItemInfo &options(QStringList options) {
        m_options = options;
        return *this;
    }
    WConfigItemInfo &addOption(QString options) {
        m_options.append(options);
        return *this;
    }
    WConfigItemInfo &callback(ActionCallback cb) {
        m_callback = cb;
        return *this;
    }
    // 访问器
    QString displayName() const { return m_displayName; }
    QString description() const { return m_description; }
    DataType elementType() const { return m_elementType; }
    Properties properties() const { return m_properties; }
    int decimalPlaces() const { return m_decimalPlaces; }
    QString defaultItem() const { return m_defaultItem; }
    QVariant defaultValue() const { return m_defaultValue; }
    QStringList options() const { return m_options; }
    ActionCallback callback() const { return m_callback; }

protected:
    friend class WConfigDataBase;
    QString m_displayName;
    QString m_description;
    DataType m_elementType = DataType::None;
    Properties m_properties;
    int m_decimalPlaces = 2;
    QString m_defaultItem;
    QVariant m_defaultValue;
    QStringList m_options;
    ActionCallback m_callback;
};

} // namespace we::config

#endif // WCONFIGITEMINFO_H