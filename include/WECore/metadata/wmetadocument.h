/**
 * @file wmetadocument.h
 * @brief Configuration document class for JSON import/export.
 *
 * WMetaDocument extends WMetaData<QVariant> with the ability to load
 * key‑value pairs from a JSON file or string, and to save them back.
 *
 * @author howdy213
 * @date 2026-05-01
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
#ifndef WMETADOCUMENT_H
#define WMETADOCUMENT_H

#include <QJsonObject>
#include <QMap>
#include <QString>
#include <QVariant>

#include "WECore/Def/wedef.h"
#include "WECore/metadata/wmetadata.h"

namespace we {

/**
 * @brief A JSON‑backed configuration document.
 *
 * Inherits the key‑value storage of WMetaData<QVariant> and adds
 * serialisation to/from JSON files or strings. Only top‑level keys
 * are supported; nested objects are not split automatically.
 */
class WE_EXPORT WMetaDocument : public WMetaData<QVariant>
{
public:
    WMetaDocument() = default;
    ~WMetaDocument() override = default;

    /**
     * @brief Loads configuration from a JSON source.
     * @param source  Either a file path or raw JSON text, depending on @p isPath.
     * @param isPath  If @c true, @p source is treated as a path to a UTF‑8
     *                text file; otherwise as a JSON string.
     * @return @c true on success, @c false on file I/O error or JSON parse error.
     */
    bool load(const QString &source, bool isPath);

    /**
     * @brief Saves the current configuration (user‑set values) to a JSON file.
     * @param filePath  Destination file path (UTF‑8).
     * @return @c true if the file was written successfully.
     */
    bool save(const QString &filePath) const;

    /**
     * @brief Exports the current configuration as a compact JSON string.
     * @return JSON text containing only user‑set key‑value pairs.
     */
    QString toJsonString() const;

private:
    /**
     * @brief Converts a flat QMap of QVariants to a QJsonObject.
     *
     * This is a simple 1:1 mapping; dot‑separated nesting is not performed.
     * @param map  The flat map to convert.
     * @return A QJsonObject with the same keys and values.
     */
    static QJsonObject mapToJson(const QMap<QString, QVariant> &map);
};

} // namespace we

Q_DECLARE_METATYPE(we::WMetaDocument)
Q_DECLARE_METATYPE(we::WMetaDocument *)

#endif // WMETADOCUMENT_H