/**
 * @file weclass.cpp
 * @brief WE类管理实现文件
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
#include "../WConfig/wconfigdocument.h"
#include "../WE/webase.h"
#include "../WFile/wpath.h"
#include "../WPlugin/wpluginmanager.h"
#include "../WPlugin/wwidgetmanager.h"

#include "weclass.h"
using namespace we::Consts;

///
/// \brief The WEClassPrivate class
///
class WEClassPrivate {
public:
    WEBase *base = 0;
};
///
/// \brief WEClass::WEClass
/// \param base
///
WEClass::WEClass(WEBase *base) {
    d = new WEClassPrivate;
    d->base = base;
}
///
/// \brief WEClass::~WEClass
///
WEClass::~WEClass() {
    delete d;
    d = nullptr;
}
///
/// \brief WEClass::configManager
/// \return
///
WConfigDocument *WEClass::configManager() {
    return d->base->getWEBaseData()->getData<WConfigDocument *>(Public::Config);
}
///
/// \brief WEClass::pathManager
/// \return
///
WPath *WEClass::pathManager() {
    return d->base->getWEBaseData()->getData<WPath *>(Public::Path);
}
///
/// \brief WEClass::pluginManager
/// \return
///
WPluginManager *WEClass::pluginManager() {
    return d->base->getWEBaseData()->getData<WPluginManager *>(Public::PManager);
}
///
/// \brief WEClass::widgetManager
/// \return
///
WWidgetManager *WEClass::widgetManager() {
    return d->base->getWEBaseData()->getData<WWidgetManager *>(Public::WManager);
}
