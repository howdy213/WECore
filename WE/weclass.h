/**
 * @file weclass.h
 * @brief WE类管理头文件，声明了各类管理器的获取接口
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
#ifndef WECLASS_H
#define WECLASS_H
#include "../WDef/wedef.h"

class WEClassPrivate;

W_INLINE namespace WE_NAMESPACE{
class WE_EXPORT WEClass{
public:
    WEClass(WEBase *base = nullptr);
    virtual ~WEClass();
    virtual WConfigDocument *configManager();
    virtual WPath *pathManager();
    virtual WPluginManager *pluginManager();
    virtual WWidgetManager *widgetManager();

private:
    WEClassPrivate *d = nullptr;
};
}
#endif // WECLASS_H
