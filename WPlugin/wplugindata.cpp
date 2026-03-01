/**
 * @file wplugindata.cpp
 * @brief 插件数据类
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
#include "wplugindata.h"

///
/// \brief The PluginDataPrivate class
///
class PluginDataPrivate
{
public:
    WEBase* data=nullptr;
    WPlugin* plugin=nullptr;
};
PluginDataPrivate* PluginData::d=nullptr;
///
/// \brief PluginData::PluginData
///
PluginData::PluginData() {
}
///
/// \brief PluginData::~PluginData
///
PluginData::~PluginData(){
}
///
/// \brief PluginData::setData
/// \param data
///
void PluginData::setData(WEBase* data){
    if(!d)d=new PluginDataPrivate;
    d->data=data;
}
///
/// \brief PluginData::getData
/// \return
///
WEBase* PluginData::getData(){
    if(!d)d=new PluginDataPrivate;
    return d->data;
}
///
/// \brief PluginData::setPlugin
/// \param plugin
///
void PluginData::setPlugin(WPlugin* plugin){
    if(!d)d=new PluginDataPrivate;
    d->plugin=plugin;
}
///
/// \brief PluginData::getPlugin
/// \return
///
WPlugin* PluginData::getPlugin(){
    if(!d)d=new PluginDataPrivate;
    return d->plugin;
}
