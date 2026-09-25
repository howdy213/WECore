/**
 * @file webasedata.h
 * @brief Header file for WE base data.
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
#ifndef WEBASEDATA_H
#define WEBASEDATA_H
#include <QMap>
#include <QObject>
#include <QString>
#include <QVariant>

#include "WECore/def/wedef.h"

namespace we {
class WEBaseDataPrivate;
/**
 * @class WEBaseData
 * @brief Untyped key/value container shared by the WE system.
 *
 * Values are stored as QVariant and looked up by string key. An optional
 * alias map lets a lookup key be redirected to another key. Not a QObject.
 */
class WE_EXPORT WEBaseData {
public:
    WEBaseData();
    virtual ~WEBaseData();
    template <typename T> T getData(QString name);
    QVariant getData(QString name);
    void addData(QString key, QVariant value);
    void addMapping(QString preimage, QString image);

private:
    WEBaseDataPrivate *d = nullptr;
};

/// Typed convenience overload: casts the stored QVariant to @c T.
template <typename T> T WEBaseData::getData(QString name) {
    return qvariant_cast<T>(getData(name));
}
} // namespace we
#endif // WEBASEDATA_H