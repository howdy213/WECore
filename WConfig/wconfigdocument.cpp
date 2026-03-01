/**
 * @file wconfigdocument.cpp
 * @brief 配置文档实现文件
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

///
/// \brief WConfigDocument::WConfigDocument
///
WConfigDocument::WConfigDocument() {}
///
/// \brief WConfigDocument::~WConfigDocument
///
WConfigDocument::~WConfigDocument() {}
///
/// \brief WConfigDocument::load
/// \param text
/// \param isPath
/// \return
///
bool WConfigDocument::load(QString text, bool isPath) {
    QString json = "";
    if (isPath) {
        QFile file(text);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            in.setEncoding(QStringConverter::Utf8);
            json = in.readAll();
            file.close();
        } else
            return false;
    } else
        json = text;
    QJsonParseError jsonError;
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(json.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError && !doc.isNull()) {
        qDebug() << "Json格式错误！" << jsonError.error;
        return false;
    }
    QJsonObject obj = doc.object();
    for (auto it = obj.begin(); it != obj.end(); it++) {
        this->set(it.key(), it.value().toVariant());
    }
    return true;
}
///
/// \brief WConfigDocument::toJson
/// \return
///
QString WConfigDocument::toJson() {
    QJsonDocument doc2 = QJsonDocument::fromVariant(toMap());
    return doc2.toJson();
}
