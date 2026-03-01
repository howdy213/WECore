/**
 * @file webase.h
 * @brief WE基础类头文件
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
#ifndef WEBASE_H
#define WEBASE_H
#include <QObject>
#include <QSharedPointer>

#include "../WDef/wedef.h"
#include "../WE/webasedata.h"
#include "../WE/weclass.h"

class WEBasePrivate;

W_INLINE namespace WE_NAMESPACE{
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
    WEBasePrivate *d = 0;
};
}
Q_DECLARE_METATYPE(WE_NAMESPACE::WEBase);
Q_DECLARE_METATYPE(WE_NAMESPACE::WEBase *);

#endif // WEBASE_H
