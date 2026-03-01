/**
 * @file webasedata.cpp
 * @brief WE基础数据实现文件
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
#include "webasedata.h"

class WEBaseDataPrivate {
public:
    QMap<QString, QVariant> data;
    QMap<QString, QString> map;//QString到QString的映射，处理别名
};

///
/// \brief WEBaseData::WEBaseData
///
WEBaseData::WEBaseData() { d = new WEBaseDataPrivate; }
///
/// \brief WEBaseData::~WEBaseData
///
WEBaseData::~WEBaseData() {
    delete d;
    d = nullptr;
}

QVariant WEBaseData::getData(QString name)
{
    name = d->map.contains(name) ? d->map[name] : name;
    if(d->data.contains(name))
        return d->data[name];
    return QVariant();
}
///
/// \brief WEBaseData::addData
/// \param key
/// \param value
///
void WEBaseData::addData(QString key, QVariant value) {
    d->data.insert(key, value);
}
