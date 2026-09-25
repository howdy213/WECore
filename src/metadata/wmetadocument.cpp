/**
 * @file wmetadocument.cpp
 * @brief Implementation of WMetaDocument.
 *
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
#include "WECore/metadata/wmetadocument.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QTextStream>

namespace we {


// Loading


bool WMetaDocument::load(const QString &source, bool isPath)
{
    QString jsonText;
    if (isPath) {
        QFile file(source);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << tr("WMetaDocument::load: Failed to open file:") << source;
            return false;
        }
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        jsonText = in.readAll();
        file.close();
    } else {
        jsonText = source;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << tr("WMetaDocument::load: JSON parse error at offset")
                   << parseError.offset << ":" << parseError.errorString();
        return false;
    }

    // An empty or null document is not an error; just leave the map unchanged.
    if (doc.isNull())
        return true;

    const QJsonObject obj = doc.object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        set(it.key(), it.value().toVariant());
    }
    return true;
}


// Saving


bool WMetaDocument::save(const QString &filePath) const
{
    // Merge into the existing file: other writers (e.g. WConfig) may share this
    // file, so only the keys owned by this document are replaced.
    QJsonObject root;
    QFile existingFile(filePath);
    if (existingFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const QJsonDocument existing =
            QJsonDocument::fromJson(existingFile.readAll());
        if (existing.isObject())
            root = existing.object();
        existingFile.close();
    }

    const QJsonObject own = mapToJson(toMap());
    for (auto it = own.begin(); it != own.end(); ++it) {
        root[it.key()] = it.value();
    }

    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << tr("WMetaDocument::save: Failed to open file for writing:") << filePath;
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}


// JSON conversion helpers


QJsonObject WMetaDocument::mapToJson(const QMap<QString, QVariant> &map)
{
    QJsonObject obj;
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        obj[it.key()] = QJsonValue::fromVariant(it.value());
    }
    return obj;
}

QString WMetaDocument::toJsonString() const
{
    QJsonObject obj = mapToJson(toMap());
    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

} // namespace we