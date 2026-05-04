/**
 * @file weclass.cpp
 * @brief Implementation file for the WE class manager.
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
#include "WECore/file/wpath.h"
#include "WECore/metadata/wmetadocument.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/we/webase.h"
#include "WECore/widget/wwidgetmanager.h"
#include "WECore/service/wserviceregistry.h"

#include "WECore/we/weclass.h"

using namespace we::Consts;
namespace we {

/**
 * @class WEClassPrivate
 * @brief Private implementation class for WEClass.
 */
class WEClassPrivate {
public:
    WEBase *base = nullptr;
};

/**
 * @brief Constructs a WEClass object.
 * @param base Pointer to the WEBase instance.
 */
WEClass::WEClass(WEBase *base) {
    d = new WEClassPrivate;
    d->base = base;
}

/**
 * @brief Destroys the WEClass object.
 */
WEClass::~WEClass() {
    delete d;
    d = nullptr;
}

/**
 * @brief Returns the configuration manager.
 * @return Pointer to the WMetaDocument instance managing configuration.
 */
WMetaDocument *WEClass::configManager() {
    return d->base->getWEBaseData()->getData<WMetaDocument *>(Public::Config);
}

/**
 * @brief Returns the path manager.
 * @return Pointer to the WPath instance managing file paths.
 */
WPath *WEClass::pathManager() {
    return d->base->getWEBaseData()->getData<WPath *>(Public::Path);
}

/**
 * @brief Returns the plugin manager.
 * @return Pointer to the WPluginManager instance.
 */
WPluginManager *WEClass::pluginManager() {
    return d->base->getWEBaseData()->getData<WPluginManager *>(Public::PManager);
}

/**
 * @brief Returns the widget manager.
 * @return Pointer to the WWidgetManager instance.
 */
WWidgetManager *WEClass::widgetManager() {
    return d->base->getWEBaseData()->getData<WWidgetManager *>(Public::WManager);
}

/**
 * @brief Returns the service registry.
 * @return Pointer to the WServiceRegistry instance.
 */
WServiceRegistry *WEClass::serviceRegistry() {
    return d->base->getWEBaseData()->getData<WServiceRegistry *>(
        Public::ServiceRegistry);
}

} // namespace we