/**
 * @file wconfigdocument.h
 * @brief Configuration document class for JSON import/export
 * @author howdy213
 * @date 2026-1-30
 * @version 1.1.0
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
#ifndef WCONFIGDOCUMENT_H
#define WCONFIGDOCUMENT_H

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QString>
#include <QTextStream>
#include <QVariant>

#include "wconfig.h"
W_INLINE namespace we {
    class WE_EXPORT WConfigDocument : public WConfig<QVariant> {
    public:
    WConfigDocument() = default;
    virtual ~WConfigDocument() = default;

    /**
     * @brief Load configuration from a JSON source.
     * @param source JSON string or file path (depending on @p isPath)
     * @param isPath If true, @p source is interpreted as a file path; otherwise
     * as raw JSON text.
     * @return true on success, false on parse error or file I/O error.
     */
    bool load(const QString &source, bool isPath);

    /**
     * @brief Save current configuration to a JSON file.
     * @param filePath Destination file path.
     * @return true if the file was written successfully.
     */
    bool save(const QString &filePath) const;

    /**
     * @brief Convert current configuration to a compact JSON string.
     * @return JSON representation of the configuration (user-set values only).
     */
    QString toJsonString() const;

private:
    /**
     * @brief Convert a flat QMap to a nested QJsonObject.
     *        Supports dot-separated keys (e.g., "database.host") for nested
     * objects.
     */
    static QJsonObject mapToJson(const QMap<QString, QVariant> &map);
    };
}
Q_DECLARE_METATYPE(WE_NAMESPACE::WConfigDocument)
Q_DECLARE_METATYPE(WE_NAMESPACE::WConfigDocument *)

#endif // WCONFIGDOCUMENT_H