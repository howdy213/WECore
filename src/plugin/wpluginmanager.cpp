/**
 * @file wpluginmanager.cpp
 * @brief Implementation of WPluginManager.
 *
 * Contains the private data class and all method definitions.
 *
 * @author howdy213
 * @date 2026-05-01
 * @version 2.0.0
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

#include "WECore/plugin/wplugin.h"
#include "WECore/plugin/wplugininterface.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/we/we.h"
#include "WECore/we/webase.h"
#include "WECore/widget/wwidgetmanager.h"

using namespace we::Consts;

namespace we {

/**
 * @brief Private data of WPluginManager (d‑pointer pattern).
 *
 * Stores the main plugin registry and keeps internal state hidden
 * from the public header.
 */
class WPluginManagerPrivate
{
public:
    /// Maps plugin UUID to the plugin instance.
    QHash<QUuid, WPlugin *> plugins;
};


// Construction / Destruction
/// Creates an empty manager.
WPluginManager::WPluginManager(QObject *parent)
    : QObject(parent)
    , d_ptr(new WPluginManagerPrivate)
{
}

/// Destructor. The QScopedPointer automatically deletes the private data.
/// Plugins should have been unloaded before destruction.
WPluginManager::~WPluginManager() = default;


// Plugin registration
/// Registers a plugin, assigning a unique ID and ensuring a unique name.
bool WPluginManager::addPlugin(WPlugin *plugin)
{
    Q_D(WPluginManager);
    if (!plugin)
        return false;

    const QUuid newId = QUuid::createUuid();
    const QString baseName = qvariant_cast<QString>(plugin->getMetaData(Plugin::Name));
    const QString uniqueName = makeUniquePluginName(baseName, QUuid());
    plugin->setMetaData(Plugin::Name, uniqueName);

    d->plugins.insert(newId, plugin);
    return true;
}

// Loading / Unloading
/// Loads a plugin’s backend. Returns false if the plugin is not registered.
bool WPluginManager::loadPlugin(WPlugin *plugin)
{
    if (!plugin)
        return false;
    if (getUuid(plugin).isNull())
        return false;
    if (plugin->available())
        return true;   // Already loaded

    // TODO: Check plugin dependencies before loading.
    return plugin->load();
}

/// Unloads a specific plugin and removes it from the registry.
bool WPluginManager::unloadPlugin(WPlugin *plugin)
{
    Q_D(WPluginManager);
    if (!plugin)
        return false;

    const QUuid id = getUuid(plugin);
    if (id.isNull())
        return false;

    auto it = d->plugins.find(id);
    if (it == d->plugins.end())
        return false;

    if (!it.value()->unload())
        return false;

    d->plugins.erase(it);
    return true;
}

/// Unloads every plugin. Uses a list of UUIDs to safely iterate while erasing.
void WPluginManager::unloadAllPlugins()
{
    Q_D(WPluginManager);
    const QVector<QUuid> ids = d->plugins.keys().toVector();
    for (const QUuid &id : ids) {
        auto it = d->plugins.find(id);
        if (it != d->plugins.end()) {
            if (it.value()->unload())
                d->plugins.erase(it);
        }
    }
}


// Plugin initialisation
/// Initialises a plugin, setting up widget integration and invoking the callback.
bool WPluginManager::initPlugin(WPlugin *plugin, InitDataProc proc)
{
    if (!plugin)
        return false;
    if (!plugin->available())
        return false;
    if (getUuid(plugin).isNull())
        return false;

    const QString type = qvariant_cast<QString>(plugin->getMetaData(Plugin::Type));

    // EXE plugins may skip initialisation if the "Init" key is "default".
    if (type == QLatin1String("exe")) {
        const QString initFlag =
            qvariant_cast<QString>(plugin->getMetaData(Plugin::Init));
        if (initFlag == QLatin1String("default"))
            return false;
    }

    auto *widgetMgr = WApp->getWEClass()->widgetManager();
    WPluginInterface *iface = plugin->inst();
    if (!iface)
        return false;

    WMessage data;
    QVariant varBase, varPlugin;
    varBase.setValue(static_cast<WEBase *>(WE::inst()));
    varPlugin.setValue(plugin);
    data.map.insert(Data::WEBase, varBase);
    data.map.insert(Data::Plugin, varPlugin);
    data.object = new QObject(this);

    // Register the plugin widget with the global widget manager.
    widgetMgr->addWidget(plugin->getId(), data.object, plugin->inst());
    widgetMgr->setAttr(data.object, Widget::Name, plugin->getMetaData(Plugin::Name));
    widgetMgr->setAttr(data.object, Widget::ParentUuid, plugin->getId());

    proc(data);
    return iface->init(data);
}


// Queries
/// Returns all plugins matching a key/value metadata pair.
QVector<QUuid> WPluginManager::getPluginByAttr(const QString &key,
                                               const QVariant &value) const
{
    Q_D(const WPluginManager);
    QVector<QUuid> result;
    for (auto it = d->plugins.cbegin(); it != d->plugins.cend(); ++it) {
        if (it.value()->getMetaData(key) == value)
            result.append(it.key());
    }
    return result;
}

/// Finds a plugin UUID by name (first match).
QUuid WPluginManager::getPluginByName(const QString &name) const
{
    const QVector<QUuid> list = getPluginByAttr(Plugin::Name, name);
    return list.isEmpty() ? QUuid() : list.first();
}

/// Returns the plugin instance for a given UUID, or nullptr.
WPlugin *WPluginManager::getPluginById(const QUuid &id) const
{
    Q_D(const WPluginManager);
    return d->plugins.value(id, nullptr);
}

/// Returns UUIDs of all registered plugins.
QVector<QUuid> WPluginManager::allPluginsId() const
{
    Q_D(const WPluginManager);
    return d->plugins.keys().toVector();
}

/// Returns pointers to all registered plugin instances.
QVector<WPlugin *> WPluginManager::allPluginsInst() const
{
    Q_D(const WPluginManager);
    return d->plugins.values().toVector();
}

/// Finds the UUID of a plugin instance.
QUuid WPluginManager::getUuid(const WPlugin *plugin) const
{
    Q_D(const WPluginManager);
    for (auto it = d->plugins.cbegin(); it != d->plugins.cend(); ++it) {
        if (it.value() == plugin)
            return it.key();
    }
    return QUuid();
}


// Metadata persistence
/// Stores metadata, applying name‑uniqueness logic if the key is Plugin::Name.
QVariant WPluginManager::setPluginData(const QUuid &id,
                                       const QString &key,
                                       const QVariant &value)
{
    Q_D(WPluginManager);
    if (!d->plugins.contains(id))
        return value;

    if (key != Plugin::Name)
        return value;

    const QString uniqueName = makeUniquePluginName(value.toString(), id);
    return QVariant::fromValue(uniqueName);
}


// Message routing


/// Sends a message to the plugin whose name matches msg.dest.
bool WPluginManager::sendMsg(WMessage &msg)
{
    WPlugin *plugin = getPluginById(getPluginByName(msg.dest));
    if (!plugin)
        return false;

    WPluginInterface *iface = plugin->inst();
    if (!iface)
        return false;

    iface->recMsg(msg);
    return true;
}

QString WPluginManager::makeUniquePluginName(const QString &baseName,
                                             const QUuid &excludeId) const
{
    Q_D(const WPluginManager);

    // Collect all names currently in use, skipping the plugin being renamed.
    QSet<QString> used;
    for (auto it = d->plugins.cbegin(); it != d->plugins.cend(); ++it) {
        if (it.key() == excludeId)
            continue;
        const QString name = qvariant_cast<QString>(
            it.value()->getMetaData(Plugin::Name));
        if (!name.isEmpty())
            used.insert(name);
    }

    // If the desired name is already unique, return it.
    if (!used.contains(baseName))
        return baseName;

    // Extract the root name – strip any existing " (number)" suffix.
    static const QRegularExpression suffixRegex(
        QRegularExpression::anchoredPattern(QStringLiteral(R"((.*) \((\d+)\))")));
    QRegularExpressionMatch match = suffixRegex.match(baseName);
    QString root = match.hasMatch() ? match.captured(1) : baseName;

    // Generate "root (2)", "root (3)", … until an unused name is found.
    // Number 1 is never created to keep the naming scheme clean.
    int suffix = 2;
    QString candidate;
    do {
        candidate = QStringLiteral("%1 (%2)").arg(root).arg(suffix);
        ++suffix;
    } while (used.contains(candidate));

    return candidate;
}
} // namespace we