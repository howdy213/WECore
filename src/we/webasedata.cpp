/**
 * @file webasedata.cpp
 * @brief Implementation file for WE base data.
 * @author howdy213
 * @date 2026-09-25
 * @version 2.1.0
 *
 * @copyright Copyright 2025-2026 howdy213
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
#include "WECore/we/webasedata.h"

namespace we {
/**
 * @class WEBaseDataPrivate
 * @brief Private implementation class for WEBaseData.
 */
class WEBaseDataPrivate {
public:
    QMap<QString, QVariant> data; ///< Key/value data storage.
    QMap<QString, QString> map;   ///< Alias map: lookup key -> target key.
};

WEBaseData::WEBaseData() {
    d = new WEBaseDataPrivate;
}

WEBaseData::~WEBaseData() {
    delete d;
    d = nullptr;
}

QVariant WEBaseData::getData(QString name) {
    // Resolve an alias first, then fall back to the key itself.
    name = d->map.contains(name) ? d->map[name] : name;
    if (d->data.contains(name))
        return d->data[name];
    return QVariant();
}

void WEBaseData::addData(QString key, QVariant value) {
    d->data.insert(key, value);
}

} // namespace we