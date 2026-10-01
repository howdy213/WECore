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
#ifndef WCONFIGITEMINFO_H
#define WCONFIGITEMINFO_H

#include "WConfigDef.h"
#include <QString>
#include <QStringList>
#include <QVariant>

namespace we::config {

// Metadata of a single config item: display name, description, element type (for
// arrays), properties, decimal places, default value, option list, callback and
// the rule deciding when the item is shown. Built by the template or the caller
// and handed to WConfigDataBase. Setters return *this to allow chaining.
class WE_EXPORT WConfigItemInfo {
public:
    WConfigItemInfo() = default;

    // Fluent setters
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
        // QJsonDocument yields integers as LongLong; store them as int so an
        // int-typed item keeps its natural type
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
    // Shows this item only while the item at @p path holds one of @p values; @p
    // path is relative to the directory the item lives in (a sibling's key, or
    // "sub/key" below it). Without a rule the item is always shown. The value read
    // is the temporary one, so the item follows the selection as the user makes it,
    // before anything is saved; WConfigWidget re-evaluates it whenever the UI
    // changes a temporary value.
    WConfigItemInfo &visibleWhen(const QString &path, const QStringList &values) {
        m_visibleWhenPath = path;
        m_visibleWhenValues = values;
        return *this;
    }
    // Accessors
    QString displayName() const { return m_displayName; }
    QString description() const { return m_description; }
    DataType elementType() const { return m_elementType; }
    Properties properties() const { return m_properties; }
    bool hasProperty(Property p) const { return m_properties.contains(p); }
    void addProperty(Property p) {
        if (!m_properties.contains(p))
            m_properties.append(p);
    }
    void removeProperty(Property p) {
        int idx = m_properties.indexOf(p);
        while (idx >= 0) {
            m_properties.removeAt(idx);
            idx = m_properties.indexOf(p);
        }
    }
    int decimalPlaces() const { return m_decimalPlaces; }
    QString defaultItem() const { return m_defaultItem; }
    QVariant defaultValue() const { return m_defaultValue; }
    QStringList options() const { return m_options; }
    ActionCallback callback() const { return m_callback; }
    // Rule set by visibleWhen(): empty path means the item is always shown.
    QString visibleWhenPath() const { return m_visibleWhenPath; }
    QStringList visibleWhenValues() const { return m_visibleWhenValues; }

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
    QString m_visibleWhenPath;
    QStringList m_visibleWhenValues;
};

} // namespace we::config

#endif // WCONFIGITEMINFO_H