/**
 * @file webase.h
 * @brief Header file for the WE base class.
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
#ifndef WEBASE_H
#define WEBASE_H
#include <QObject>
#include <QSharedPointer>

#include "WECore/Def/wedef.h"
#include "WECore/we/webasedata.h"
#include "WECore/we/weclass.h"

namespace we {
class WEBasePrivate;
/**
 * @class WEBase
 * @brief Base class for the WE system.
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