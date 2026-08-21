/**
 * @file wvirtualplugin.cpp
 * @brief Implementation of WVirtualPlugin.
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
#include "WECore/plugin/wvirtualplugin.h"

#include "WECore/file/wshellexecute.h"
#include "WECore/plugin/wplugin.h"
#include "WECore/plugin/wplugindata.h"
#include "WECore/we/we.h"
#include "WECore/widget/wwidgetmanager.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVariant>

using namespace we::Consts;

namespace we {

/**
 * @brief Private data for WVirtualPlugin (d‑pointer pattern).
 *
 * Stores all internal state: the executable file path, the associated
 * WPlugin metadata object, admin flag, topic list, and the pointer
 * to the global event bus.
 */
class WVirtualPluginPrivate {
public:
    QString m_filePath;          ///< Path to the executable.
    WPlugin *m_plugin = nullptr; ///< Associated metadata plugin.
    bool m_admin = false;        ///< Whether admin rights should be used.
    QStringList m_topics;        ///< Subscribed event topics.
    WWidgetManager *m_bus =
        nullptr; ///< Pointer to the event bus (widget manager).
};

// Construction / Destruction

/// Constructs the virtual plugin with an empty private data object.
WVirtualPlugin::WVirtualPlugin(QObject *parent)
    : QObject(parent), d_ptr(new WVirtualPluginPrivate) {}

/// Destructor: unsubscribes from all events, then the private data is
/// automatically deleted.
WVirtualPlugin::~WVirtualPlugin() {
    Q_D(WVirtualPlugin);
    if (d->m_bus) {
        d->m_bus->unsubscribeAll(this);
        d->m_bus = nullptr;
    }
}

// Configuration

void WVirtualPlugin::setFile(const QString &filePath) {
    Q_D(WVirtualPlugin);
    d->m_filePath = filePath;
}

QString WVirtualPlugin::getFilePath() const {
    Q_D(const WVirtualPlugin);
    return d->m_filePath;
}

void WVirtualPlugin::setPlugin(WPlugin *plugin) {
    Q_D(WVirtualPlugin);
    d->m_plugin = plugin;
}

void WVirtualPlugin::setTopics(const QStringList &topics) {
    Q_D(WVirtualPlugin);
    d->m_topics = topics;
}

QStringList WVirtualPlugin::getTopics() const {
    Q_D(const WVirtualPlugin);
    return d->m_topics;
}

// Initialisation
/// Initialises the virtual plugin: selects topics, subscribes, and may launch
/// the executable.
bool WVirtualPlugin::init(WMessage &msg) {
    Q_UNUSED(msg);
    Q_D(WVirtualPlugin);

    if (!d->m_plugin) {
        qWarning()
        << "WVirtualPlugin::init called without a plugin metadata object";
        return false;
    }

    WEBase *base = WApp;
    auto weClass = base->getWEClass();
    if (!weClass)
        return false;
    d->m_bus = weClass->widgetManager();

    if (d->m_topics.isEmpty()) {
        // Listen on VirtualPlugin.<name>
        if (d->m_topics.isEmpty()) {
            const QString name = d->m_plugin->name();
            if (!name.isEmpty())
                d->m_topics << QStringLiteral("VirtualPlugin.%1").arg(name);
        }
    }

    subscribeTopics();
    d->m_admin = d->m_plugin->hasMetaData(Plugin::Admin);
    const QString initArgs = d->m_plugin->initArg();
    if (!initArgs.isEmpty() && initArgs != QLatin1String("default")) {
        const QString op =
            d->m_admin ? QStringLiteral("runas") : QStringLiteral("open");
        WShellExecute::asyncExecute(d->m_filePath, op, initArgs);
    }

    return true;
}

// Event subscription
/// Subscribes to every topic in the list.
void WVirtualPlugin::subscribeTopics() {
    Q_D(WVirtualPlugin);
    if (!d->m_bus || d->m_topics.isEmpty())
        return;

    for (const QString &topic : std::as_const(d->m_topics)) {
        d->m_bus->subscribe(
            topic, this,
            (SubscribeFunc)[this](const WEvent &event) { onEventReceived(event); });
    }
    qDebug() << "WVirtualPlugin subscribed to topics:" << d->m_topics;
}

// Event handling
/// Converts an event to command‑line arguments and launches the executable.
void WVirtualPlugin::onEventReceived(const WEvent &event) {
    Q_D(WVirtualPlugin);
    if (d->m_filePath.isEmpty())
        return;

    d->m_admin = d->m_plugin->hasMetaData(Plugin::Admin);
    const QString op =
        d->m_admin ? QStringLiteral("runas") : QStringLiteral("open");

    // Build the argument string.
    QString params;
    params += event.msg.command;

    WShellExecute::asyncExecute(d->m_filePath, op, params);
}

// Deinitialisation
/// Unsubscribes from all events and releases the event bus pointer.
bool WVirtualPlugin::deinit(WMessage &msg) {
    Q_UNUSED(msg);
    Q_D(WVirtualPlugin);
    if (d->m_bus) {
        d->m_bus->unsubscribeAll(this);
        d->m_bus = nullptr;
    }
    return true;
}

// Message reception
/// Ignored; all communication is handled through the event bus.
void WVirtualPlugin::recMsg(WMessage &msg) {
    Q_UNUSED(msg);
    // Not used in the event‑bus architecture.
}

} // namespace we