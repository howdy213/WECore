/**
 * @file wplugininterface.h
 * @brief 插件接口头文件
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
#ifndef WPLUGININTERFACE_H
#define WPLUGININTERFACE_H
#include "../WDef/wedef.h"
#include "../WPlugin/wpluginmanager.h"
#include "../WPlugin/wpluginmetadata.h"

#define W_DECLARE_PLUGIN(ClassType)                                                 \
public:                                                                             \
bool connectManager(WPluginManager *manager, bool isConnect) const override {       \
if (isConnect)                                                                      \
    return connect(this, &ClassType::sendMsg, manager, &WPluginManager::sendMsg);   \
else                                                                                \
    return disconnect(this, &ClassType::sendMsg, manager, &WPluginManager::sendMsg);\
};

class WPluginInterface {
public:
    virtual ~WPluginInterface() {}

public:
    virtual bool init(WMetaData &msg) = 0;
    virtual void recMsg(WMetaData &msg) = 0;
    virtual bool deinit(WMetaData &msg) = 0;
    virtual bool connectManager(WPluginManager *receiver,
                                bool isConnect = true) const = 0;
};

#define WPluginInterfaceIID "QPlugins.WPluginManager.WPluginInterface"
Q_DECLARE_INTERFACE(WPluginInterface,WPluginInterfaceIID);

#endif // WPLUGININTERFACE_H
