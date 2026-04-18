/**
 * @file wconfigdocument.cpp
 * @brief Configuration document implementation
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

#include "wconfigdocument.h"

#include <QDebug>

///
/// \brief WConfigDocument::load
/// \param source JSON string or file path
/// \param isPath If true, treat source as file path; otherwise as raw JSON text
/// \return True on success, false on error
///
bool WConfigDocument::load(const QString &source, bool isPath) {
    QString jsonText;
    if (isPath) {
        QFile file(source);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open file:" << source;
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
        qWarning() << "JSON parse error at offset" << parseError.offset << ":" << parseError.errorString();
        return false;
    }

    if (doc.isNull()) {
        return true;
    }

    QJsonObject obj = doc.object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        set(it.key(), it.value().toVariant());
    }
    return true;
}

///
/// \brief WConfigDocument::save
/// \param filePath Path to save the configuration file
/// \return True on success, false on error
///
bool WConfigDocument::save(const QString &filePath) const {
    QJsonObject root = mapToJson(toMap());
    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

///
/// \brief WConfigDocument::mapToJson
/// \param map Map to convert
/// \return JSON object representing the map (only top-level keys)
///
QJsonObject WConfigDocument::mapToJson(const QMap<QString, QVariant> &map) {
    QJsonObject obj;
    for (auto it = map.begin(); it != map.end(); ++it) {
        obj[it.key()] = QJsonValue::fromVariant(it.value());
    }
    return obj;
}

///
/// \brief WConfigDocument::toJsonString
/// \return JSON string representation of the current configuration
///
QString WConfigDocument::toJsonString() const {
    QJsonDocument doc = QJsonDocument::fromVariant(toMap());
    return doc.toJson(QJsonDocument::Compact);
}