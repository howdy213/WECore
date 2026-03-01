/**
 * @file wconfigdocument.h
 * @brief 配置文档类头文件，用于解析和生成 JSON 格式的配置数据
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

class WE_EXPORT WE_NAMESPACE::WConfigDocument : public WConfig<QVariant> {
public:
    WConfigDocument();
    virtual ~WConfigDocument();
    bool load(QString text, bool isPath);
    QString toJson();
};

Q_DECLARE_METATYPE(WE_NAMESPACE::WConfigDocument)
Q_DECLARE_METATYPE(WE_NAMESPACE::WConfigDocument *)

#endif // WCONFIGDOCUMENT_H
