/**
 * @file webase.h
 * @brief Header file for the WE base class.
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
#ifndef WEBASE_H
#define WEBASE_H
#include <QObject>
#include <QSharedPointer>

#include "WECore/def/wedef.h"
#include "WECore/we/webasedata.h"
#include "WECore/we/weclass.h"

namespace we {
class WEBasePrivate;
/**
 * @class WEBase
 * @brief Application base: owns the WEClass manager factory and the
 *        WEBaseData container, plus a free-form class name.
 *
 * WEBase is the object installed in WE's global slot. It is not a QObject;
 * the WEClass and WEBaseData instances it hands out are shared-ownership.
 */
class WE_EXPORT WEBase {
public:
    WEBase();
    virtual ~WEBase();
    virtual QSharedPointer<WEClass> getWEClass();
    virtual void setWEClass(QSharedPointer<WEClass> weclass);
    virtual QSharedPointer<WEBaseData> getWEBaseData();
    virtual void setWEBaseData(QSharedPointer<WEBaseData> webasedata);
    virtual void setClassName(QString name);
    virtual QString getClassName();

private:
    WEBasePrivate *d = nullptr;
};
} // namespace we

Q_DECLARE_METATYPE(we::WEBase);
Q_DECLARE_METATYPE(we::WEBase *);

#endif // WEBASE_H