/**
 * @file webasedata.cpp
 * @brief Implementation file for WE base data.
 * @author howdy213
 * @date 2026-05-04
 * @version 2.0.0
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
    QMap<QString, QVariant> data;       ///< Key-value data storage.
    QMap<QString, QString> map;         ///< Alias mapping from preimage to image.
};

/**
 * @brief Constructs a WEBaseData object.
 */
WEBaseData::WEBaseData() {
    d = new WEBaseDataPrivate;
}

/**
 * @brief Destroys the WEBaseData object.
 */
WEBaseData::~WEBaseData() {
    delete d;
    d = nullptr;
}

/**
 * @brief Retrieves data by key, resolving aliases.
 * @param name The key (or alias) to look up.
 * @return The stored QVariant, or an invalid QVariant if not found.
 */
QVariant WEBaseData::getData(QString name) {
    name = d->map.contains(name) ? d->map[name] : name;
    if (d->data.contains(name))
        return d->data[name];
    return QVariant();
}

/**
 * @brief Adds or updates data for a given key.
 * @param key The key under which to store the data.
 * @param value The value to store.
 */
void WEBaseData::addData(QString key, QVariant value) {
    d->data.insert(key, value);
}

} // namespace we