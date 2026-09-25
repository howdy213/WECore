/**
 * @file weclass.h
 * @brief Header file for the WE class manager, declaring interfaces for accessing various managers.
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
#ifndef WECLASS_H
#define WECLASS_H
#include "WECore/def/wedef.h"

namespace we {
class WEClassPrivate;
/**
 * @class WEClass
 * @brief Factory that exposes the core managers of the WE system.
 *
 * The managers themselves are owned by the associated WEBase (stored in its
 * WEBaseData container under well-known keys); WEClass only retrieves them.
 * The returned pointers are borrowed, not owned.
 */
class WE_EXPORT WEClass {
public:
    explicit WEClass(WEBase *base = nullptr);
    virtual ~WEClass();
    virtual WMetaDocument *configManager();
    virtual WPath *pathManager();
    virtual WPluginManager *pluginManager();
    virtual WWidgetManager *widgetManager();
    virtual WServiceRegistry *serviceRegistry();

private:
    WEClassPrivate *d = nullptr;
};
} // namespace we
#endif // WECLASS_H