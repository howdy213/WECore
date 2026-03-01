/**
 * @file webase.cpp
 * @brief WE基础类实现文件
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
#include "webase.h"

using namespace WE_NAMESPACE;
class WEBasePrivate {
public:
    WEBasePrivate(WEBase *base)
        : weclass(QSharedPointer<WEClass>(new WEClass(base))),
        webasedata(QSharedPointer<WEBaseData>(new WEBaseData)) {};
    QSharedPointer<WEClass> weclass;
    QSharedPointer<WEBaseData> webasedata;
    QString wename = "";
};

///
/// \brief WEBase::WEBase
///
WEBase::WEBase() { this->d = new WEBasePrivate(this); }
///
/// \brief WEBase::~WEBase
///
WEBase::~WEBase() {
    if (d)
        delete d;
    d = nullptr;
}
///
/// \brief WEBase::getWEClass
/// \return
///
QSharedPointer<WEClass> WEBase::getWEClass() { return d->weclass; }
///
/// \brief WEBase::setWEClass
/// \param weclass
///
void WEBase::setWEClass(QSharedPointer<WEClass> weclass) {
    d->weclass = weclass;
}
///
/// \brief WEBase::getWEBaseData
/// \return
///
QSharedPointer<WEBaseData> WEBase::getWEBaseData() { return d->webasedata; }
///
/// \brief WEBase::setWEBaseData
/// \param webasedata
///
void WEBase::setWEBaseData(QSharedPointer<WEBaseData> webasedata) {
    d->webasedata = webasedata;
}
///
/// \brief WEBase::setClassName
/// \param name
///
void WEBase::setClassName(QString name) { d->wename = name; }
///
/// \brief WEBase::getClassName
/// \return
///
QString WEBase::getClassName() { return d->wename; }
