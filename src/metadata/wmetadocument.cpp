/**
 * @file wmetadocument.cpp
 * @brief Implementation of WMetaDocument.
 *
 * @author howdy213
 * @date 2026-05-01
 * @version 2.0.0
 *
 * Copyright 2025-2026 howdy213
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * ...
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
            qWarning() << "WMetaDocument::load: Failed to open file:" << source;
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
        qWarning() << "WMetaDocument::load: JSON parse error at offset"
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
    QJsonObject root = mapToJson(toMap());
    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "WMetaDocument::save: Failed to open file for writing:" << filePath;
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