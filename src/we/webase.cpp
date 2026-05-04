/**
 * @file webase.cpp
 * @brief Implementation file for the WE base class.
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
#include "WECore/we/webase.h"

namespace we {

/**
 * @class WEBasePrivate
 * @brief Private implementation class for WEBase.
 */
class WEBasePrivate {
public:
    WEBasePrivate(WEBase *base)
        : weclass(QSharedPointer<WEClass>(new WEClass(base))),
        webasedata(QSharedPointer<WEBaseData>(new WEBaseData)) {}
    QSharedPointer<WEClass> weclass;
    QSharedPointer<WEBaseData> webasedata;
    QString wename = "";
};

/**
 * @brief Constructs a WEBase object.
 */
WEBase::WEBase() {
    this->d = new WEBasePrivate(this);
}

/**
 * @brief Destroys the WEBase object.
 */
WEBase::~WEBase() {
    if (d)
        delete d;
    d = nullptr;
}

/**
 * @brief Returns the WEClass instance.
 * @return Shared pointer to the WEClass.
 */
QSharedPointer<WEClass> WEBase::getWEClass() {
    return d->weclass;
}

/**
 * @brief Sets the WEClass instance.
 * @param weclass Shared pointer to the new WEClass.
 */
void WEBase::setWEClass(QSharedPointer<WEClass> weclass) {
    d->weclass = weclass;
}

/**
 * @brief Returns the WEBaseData instance.
 * @return Shared pointer to the WEBaseData.
 */
QSharedPointer<WEBaseData> WEBase::getWEBaseData() {
    return d->webasedata;
}

/**
 * @brief Sets the WEBaseData instance.
 * @param webasedata Shared pointer to the new WEBaseData.
 */
void WEBase::setWEBaseData(QSharedPointer<WEBaseData> webasedata) {
    d->webasedata = webasedata;
}

/**
 * @brief Sets the class name.
 * @param name The class name.
 */
void WEBase::setClassName(QString name) {
    d->wename = name;
}

/**
 * @brief Returns the class name.
 * @return The class name.
 */
QString WEBase::getClassName() {
    return d->wename;
}

} // namespace we