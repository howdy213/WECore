/**
 * @file webase.cpp
 * @brief Implementation file for the WE base class.
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
#include "WECore/we/webase.h"

namespace we {

/**
 * @class WEBasePrivate
 * @brief Private implementation class for WEBase.
 */
class WEBasePrivate {
public:
    // WEClass keeps a non-owning back-pointer to its WEBase owner, so the
    // owner must outlive the WEClass it constructs.
    WEBasePrivate(WEBase *base)
        : weclass(QSharedPointer<WEClass>(new WEClass(base))),
        webasedata(QSharedPointer<WEBaseData>(new WEBaseData)) {}
    QSharedPointer<WEClass> weclass;
    QSharedPointer<WEBaseData> webasedata;
    QString wename = "";
};

WEBase::WEBase() {
    this->d = new WEBasePrivate(this);
}

WEBase::~WEBase() {
    if (d)
        delete d;
    d = nullptr;
}

QSharedPointer<WEClass> WEBase::getWEClass() {
    return d->weclass;
}

void WEBase::setWEClass(QSharedPointer<WEClass> weclass) {
    d->weclass = weclass;
}

QSharedPointer<WEBaseData> WEBase::getWEBaseData() {
    return d->webasedata;
}

void WEBase::setWEBaseData(QSharedPointer<WEBaseData> webasedata) {
    d->webasedata = webasedata;
}

void WEBase::setClassName(QString name) {
    d->wename = name;
}

QString WEBase::getClassName() {
    return d->wename;
}

} // namespace we