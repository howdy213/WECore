/**
 * @file wpluginmanager.cpp
 * @brief 插件管理器实现文件
 * @author howdy213
 * @date 2026-2-1
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
#include <QCoreApplication>
#include <QDir>
#include <QHash>
#include <QJsonArray>
#include <QPluginLoader>
#include <QVariant>

#include "../WE/we.h"
#include "../WE/webase.h"
#include "../WPlugin/wplugin.h"
#include "../WPlugin/wplugininterface.h"
#include "../WPlugin/wpluginmanager.h"
using namespace we::Consts;

///
/// \brief The WPluginManagerPrivate class
///
class WPluginManagerPrivate {
public:
    QMap<QUuid, WPlugin *> plugins;
};
///
/// \brief WPluginManager::WPluginManager
///
WPluginManager::WPluginManager() { d = new WPluginManagerPrivate; }
///
/// \brief WPluginManager::~WPluginManager
///
WPluginManager::~WPluginManager() {
    delete d;
    d = nullptr;
}
///
/// \brief WPluginManager::createPlugin
/// \param configPath
/// \param config
/// \return
///
WPlugin *WPluginManager::createPlugin(QString configPath, QString config) {
    auto plugin = new WPlugin(this);
    plugin->readConfig(configPath, config);
    return plugin;
}
///
/// \brief WPluginManager::addPlugin
/// \param plugin
/// \return
///
bool WPluginManager::addPlugin(WPlugin *plugin) {
    QString name = qvariant_cast<QString>(plugin->getMetaData(Plugin::Name));
    while (getPluginById(getPluginByName(name))) {
        name += "#";
    }
    plugin->setMetaData(Plugin::Name, name);
    d->plugins.insert(QUuid::createUuid(), plugin);
    return true;
}

bool WPluginManager::loadPlugin(WPlugin *plugin) {
    if (getUuid(plugin).isNull())
        return false;
    if (!plugin)
        return 0;
    if (plugin->available())
        return true;
    // TODO：检查依赖
    return plugin->load();
}

bool WPluginManager::initPlugin(WPlugin *plugin, InitDataProc proc) {
    if (getUuid(plugin).isNull())
        return false;
    if (!plugin->available())
        return false;
    WPluginInterface *app = plugin->inst();
    if (app) {
        if (qvariant_cast<QString>(plugin->getMetaData(Plugin::Type)) == "exe") {
            if (qvariant_cast<QString>(plugin->getMetaData(Plugin::Init)) ==
                "default") {
                return false;
            }
        }
        WMetaData data;
        QVariant var;
        QVariant var2;
        var.setValue((WEBase *)WE::inst());
        var2.setValue((WPlugin *)plugin);
        data.map.insert(Data::WEBase, var);
        data.map.insert(Data::Plugin, var2);
        proc(data);
        app->init(data);
    }
    return true;
}
///
/// \brief WPluginManager::unloadPlugin
/// \param plugin
/// \return
///
bool WPluginManager::unloadPlugin(WPlugin *plugin) {
    auto it = d->plugins.begin();
    for (; it != d->plugins.end(); it++) {
        if (*it == plugin)
            break;
    }
    if (it == d->plugins.end())
        return false;
    return unloadPlugin(it);
}
///
/// \brief WPluginManager::unloadAllPlugins
///
void WPluginManager::unloadAllPlugins() {
    for (auto it = d->plugins.begin(); it != d->plugins.end(); it++) {
        unloadPlugin(it);
    }
}
///
/// \brief WPluginManager::unloadPlugin
/// \param it
/// \return
///
bool WPluginManager::unloadPlugin(QMap<QUuid, WPlugin *>::Iterator it) {
    if (!(*it)->unload())
        return false;
    d->plugins.erase(it);
    return true;
}
///
/// \brief WPluginManager::getPluginByAttr
/// \param key
/// \param value
/// \return
///
QVector<QUuid> WPluginManager::getPluginByAttr(QString key, QVariant value) {
    QVector<QUuid> res;
    for (auto it = d->plugins.begin(); it != d->plugins.end(); it++) {
        if (it.value()->getMetaData(key) == value)
            res.push_back(it.key());
    }
    return res;
}
///
/// \brief WPluginManager::getPluginByName
/// \param name
/// \return
///
QUuid WPluginManager::getPluginByName(QString name) {
    auto list = getPluginByAttr(Plugin::Name, name);
    if (!list.isEmpty())
        return list[0];
    return QUuid();
}
//
/// \brief WPluginManager::getPlugin
/// \param id
/// \return
///
WPlugin *WPluginManager::getPluginById(QUuid id) {
    if (d->plugins.contains(id))
        return d->plugins[id];
    return nullptr;
}
///
/// \brief WPluginManager::setPluginData
/// \param id
/// \param key
/// \param value
/// \return
///
QVariant WPluginManager::setPluginData(QUuid id, QString key, QVariant value) {
    if (!d->plugins.contains(id))
        return value;
    auto plugin = d->plugins[id];
    if (key == Plugin::Name) {
        QString name = qvariant_cast<QString>(value);
        while (getPluginById(getPluginByName(name))) {
            if (getPluginByAttr(Plugin::Name, name).length() == 1 &&
                plugin->getMetaData(Plugin::Name) == name)
                return name;
            name += "#";
        }
        value = name;
    }
    return value;
}
///
/// \brief WPluginManager::getUuid
/// \param plugin
/// \return
///
QUuid WPluginManager::getUuid(WPlugin *plugin) {
    for (auto it = d->plugins.begin(); it != d->plugins.end(); it++) {
        if (it.value() == plugin)
            return it.key();
    }
    return QUuid();
}
///
/// \brief WPluginManager::allPluginsId
/// \return
///
QVector<QUuid> WPluginManager::allPluginsId() { return d->plugins.keys(); }
///
/// \brief WPluginManager::allPluginsInst
/// \return
///
QVector<WPlugin *> WPluginManager::allPluginsInst() {
    return d->plugins.values();
}
///
/// \brief WPluginManager::sendMsg
/// \param msg
///
bool WPluginManager::sendMsg(WMetaData &msg) {
    auto loader = getPluginById(getPluginByName(msg.dest));
    if (!loader)
        return false;

    auto plugin = loader->inst();
    if (!plugin)
        return false;
    plugin->recMsg(msg);
    return true;
}
