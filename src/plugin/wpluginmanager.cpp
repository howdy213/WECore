/**
 * @file wpluginmanager.cpp
 * @brief Implementation of WPluginManager.
 *
 * Contains the private data class and all method definitions.
 *
 * @author howdy213
 * @date 2026-09-25
 * @version 2.1.0
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
#include <WECore/def/wedef.h>
#include <WECore/metadata/wmetadocument.h>

#include "WECore/plugin/wplugin.h"
#include "WECore/plugin/wplugininterface.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/we/we.h"
#include "WECore/we/webase.h"
#include "WECore/widget/wwidgetmanager.h"

using namespace we::Consts;

namespace we {

/**
 * @brief Private state of WPluginManager (d-pointer pattern).
 */
class WPluginManagerPrivate {
public:
    /// Registry: instance UUID -> plugin. The manager does not own the plugins.
    QHash<QUuid, WPlugin *> plugins;
};

// Construction / Destruction

WPluginManager::WPluginManager(QObject *parent)
    : QObject(parent), d_ptr(new WPluginManagerPrivate) {}

// d_ptr frees the registry only; WPlugin instances are owned by the caller.
WPluginManager::~WPluginManager() = default;

// Plugin registration

bool WPluginManager::addPlugin(WPlugin *plugin) {
    Q_D(WPluginManager);
    if (!plugin)
        return false;

    QUuid newId = plugin->localUuid();
    if (newId.isNull())
        newId = QUuid::createUuid();
    plugin->setMetaData(Plugin::LocalUuid, newId);
    plugin->getMetaDocument().save(plugin->configPath());

    const QString baseName = plugin->name();
    const QString uniqueName = makeUniquePluginName(baseName, newId);
    plugin->setName(uniqueName);

    d->plugins.insert(newId, plugin);
    return true;
}

// Loading / Unloading

bool WPluginManager::loadPlugin(WPlugin *plugin) {
    Q_D(WPluginManager);
    if (!plugin) {
        qWarning() << tr("WPluginManager::loadPlugin: Plugin is null");
        return false;
    }

    QUuid localUuid = plugin->localUuid();
    if (!d->plugins.contains(localUuid)) {
        qWarning() << tr("WPluginManager::loadPlugin: Plugin not found in "
                         "registry: %1")
                          .arg(localUuid.toString());
        return false;
    }

    if (plugin->available()) {
        qDebug() << "WPluginManager::loadPlugin: Plugin already loaded:"
                 << plugin->name();
        return true;
    }

    // TODO: Check plugin dependencies before loading.
    bool result = plugin->load();

    return result;
}

bool WPluginManager::unloadPlugin(WPlugin *plugin) {
    Q_D(WPluginManager);
    if (!plugin)
        return false;

    const QUuid id = plugin->localUuid();
    if (id.isNull())
        return false;

    auto it = d->plugins.find(id);
    if (it == d->plugins.end())
        return false;

    if (!it.value()->unload())
        return false;

    return true;
}

bool WPluginManager::hotUnloadPlugin(WPlugin *plugin) {
    Q_D(WPluginManager);
    if (!plugin)
        return false;

    const QUuid id = plugin->localUuid();
    if (id.isNull())
        return false;

    auto it = d->plugins.find(id);
    if (it == d->plugins.end())
        return false;

    return it.value()->unload();
}

// Snapshot the keys first: erasing while iterating would invalidate iterators.
void WPluginManager::unloadAllPlugins() {
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

bool WPluginManager::initPlugin(WPlugin *plugin, InitDataProc proc) {
    Q_D(WPluginManager);
    if (!plugin)
        return false;
    if (!plugin->available())
        return false;
    if (!d->plugins.contains(plugin->localUuid()))
        return false;

    const QString type = plugin->type();

    // EXE plugins may opt out of initialisation by setting "Init" to "default".
    if (type == QLatin1String("exe")) {
        const QString initFlag = plugin->initArg();
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
    widgetMgr->addWidget(plugin->localUuid(), data.object, plugin->inst());
    widgetMgr->setAttr(data.object, Widget::Name, plugin->name());
    widgetMgr->setAttr(data.object, Widget::ParentUuid, plugin->localUuid());

    proc(data);
    return iface->init(data);
}

// Queries

QVector<QUuid> WPluginManager::getPluginByAttr(const QString &key,
                                               const QVariant &value) const {
    Q_D(const WPluginManager);
    QVector<QUuid> result;
    for (auto it = d->plugins.cbegin(); it != d->plugins.cend(); ++it) {
        if (it.value()->getMetaData(key) == value)
            result.append(it.key());
    }
    return result;
}

QUuid WPluginManager::getPluginByName(const QString &name) const {
    const QVector<QUuid> list = getPluginByAttr(Plugin::Name, name);
    return list.isEmpty() ? QUuid() : list.first();
}

WPlugin *WPluginManager::getPluginById(const QUuid &id) const {
    Q_D(const WPluginManager);
    return d->plugins.value(id, nullptr);
}

QVector<QUuid> WPluginManager::allPluginsId() const {
    Q_D(const WPluginManager);
    return d->plugins.keys().toVector();
}

QVector<WPlugin *> WPluginManager::allPluginsInst() const {
    Q_D(const WPluginManager);
    return d->plugins.values().toVector();
}

// Metadata persistence

QVariant WPluginManager::setPluginData(const QUuid &id, const QString &key,
                                       const QVariant &value) {
    Q_D(WPluginManager);
    if (!d->plugins.contains(id))
        return value;

    if (key != Plugin::Name)
        return value;

    const QString uniqueName = makeUniquePluginName(value.toString(), id);
    return QVariant::fromValue(uniqueName);
}

// Message routing

bool WPluginManager::sendMsg(WMessage &msg) {
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
                                             const QUuid &excludeId) const {
    Q_D(const WPluginManager);

    // Collect names in use, skipping the plugin that is being renamed.
    QSet<QString> used;
    for (auto it = d->plugins.cbegin(); it != d->plugins.cend(); ++it) {
        if (it.key() == excludeId)
            continue;
        const QString name = it.value()->name();
        if (!name.isEmpty())
            used.insert(name);
    }

    if (!used.contains(baseName))
        return baseName;

    // Strip any existing " (number)" suffix to recover the root name.
    static const QRegularExpression suffixRegex(
        QRegularExpression::anchoredPattern(QStringLiteral(R"((.*) \((\d+)\))")));
    QRegularExpressionMatch match = suffixRegex.match(baseName);
    QString root = match.hasMatch() ? match.captured(1) : baseName;

    // Try "root (2)", "root (3)", ... until a free name is found. Number 1 is
    // never used to keep the naming scheme uniform.
    int suffix = 2;
    QString candidate;
    do {
        candidate = QStringLiteral("%1 (%2)").arg(root).arg(suffix);
        ++suffix;
    } while (used.contains(candidate));

    return candidate;
}
} // namespace we