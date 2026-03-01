/**
 * @file wvirtualplugin.cpp
 * @brief 虚拟插件类实现文件
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
#include "wvirtualplugin.h"
#include "../WDef/wedef.h"
#include "../WFile/wshellexecute.h"
#include "../WPlugin/wplugin.h"
using namespace we::Consts;

///
/// \brief WVirtualPlugin::WVirtualPlugin
///
WVirtualPlugin::WVirtualPlugin() { return; }
///
/// \brief WVirtualPlugin::~WVirtualPlugin
///
WVirtualPlugin::~WVirtualPlugin() {}
///
/// \brief WVirtualPlugin::init
/// \param msg
///
bool WVirtualPlugin::init(WMetaData &msg) {
    Q_UNUSED(msg);
    // m_plugin=qvariant_cast<WPlugin*>(data[Data::Plugin]);
    m_admin = m_plugin->hasMetaData(Plugin::Admin);
    QString op = m_admin ? "runas" : "open";
    WShellExecute::asyncExecute(
        m_filePath, op,
        qvariant_cast<QString>(m_plugin->getMetaData(Plugin::Init)));
    return true;
}
///
/// \brief WVirtualPlugin::recMsg
/// \param msg
///
void WVirtualPlugin::recMsg(WMetaData &msg) {
    m_admin = m_plugin->hasMetaData(Plugin::Admin);
    QString op = m_admin ? "runas" : "open";
    WShellExecute::asyncExecute(m_filePath, op,
                                qvariant_cast<QString>(msg.map[Data::Command]));
}
///
/// \brief WVirtualPlugin::deinit
/// \param msg
/// \return
///
bool WVirtualPlugin::deinit(WMetaData &msg) {
    Q_UNUSED(msg);
    return true;
}
///
/// \brief WVirtualPlugin::connectManager
/// \param receiver
/// \param isConnect
/// \return
///
bool WVirtualPlugin::connectManager(WPluginManager *receiver,
                                    bool isConnect) const {
    Q_UNUSED(receiver)
    Q_UNUSED(isConnect)
    return false;
}
///
/// \brief WVirtualPlugin::csetPlugin
/// \param plugin
///
void WVirtualPlugin::setPlugin(WPlugin *plugin) { m_plugin = plugin; }
