/**
 * @file wplugindata.h
 * @brief 插件数据类
 * @author howdy213
 * @date 2026-1-30
 * @version 1.1.0
 * @details 该数据结构不应更改，且不应导出
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
#ifndef WPLUGINDATA_H
#define WPLUGINDATA_H
#include "WECore/we/webase.h"
#define PData PluginData::getData()
#define PClass PData->getWEClass()
#define PPlugin PluginData::getPlugin()

 namespace we {
    class PluginDataPrivate;
    class PluginData {
    public:
        PluginData();
        virtual ~PluginData();
        static void setData(WEBase *data);
        static WEBase *getData();
        static void setPlugin(WPlugin *plugin);
        static WPlugin *getPlugin();
        static void setWidget(WWidget *widget);
        static WWidget *getWidget();

    private:
        static PluginDataPrivate *d;
    };
}
#endif // WPLUGINDATA_H
