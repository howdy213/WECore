/**
 * @file wpluginmanager.h
 * @brief Plugin manager - loads, tracks, and communicates with plugins.
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
#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QUuid>
#include <QVariant>
#include <QVector>
#include <functional>

#include "WECore/def/wedef.h"
#include "WECore/plugin/wpluginmessage.h"

namespace we {

/**
 * @brief Callback that may fill in the initialisation message before
 * WPluginInterface::init() is invoked.
 * @param msg Mutable WMessage handed to init().
 */
using InitDataProc = std::function<void(WMessage &msg)>;

class WPluginManagerPrivate;

/**
 * @brief Central registry owning and routing messages between plugins.
 *
 * Assigns instance UUIDs, keeps plugin metadata persistent, drives the
 * load/init/unload lifecycle and routes messages by plugin name.
 *
 * @note Non-copyable. To store it inside a QVariant use a pointer
 *       (`Q_DECLARE_METATYPE(we::WPluginManager *)`).
 */
class WE_EXPORT WPluginManager : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(WPluginManager)

public:
    explicit WPluginManager(QObject *parent = nullptr);

    ~WPluginManager() override;

    /**
     * @brief Registers a plugin and assigns it a unique instance UUID.
     * @return true on success; false for a null plugin.
     *
     * The display name is made unique and the metadata is persisted to the
     * plugin's configuration file.
     */
    bool addPlugin(WPlugin *plugin);

    /**
     * @brief Loads the plugin's backend (DLL or EXE).
     * @return true if the plugin was loaded, or was already loaded; false if
     *         it is null or not registered.
     */
    bool loadPlugin(WPlugin *plugin);

    /**
     * @brief Initialises a loaded plugin and registers its widget.
     * @param plugin The plugin to initialise (must be loaded and registered).
     * @param proc   Optional callback that can extend the initialisation
     *               message.
     * @return true on success.
     *
     * EXE plugins whose "Init" metadata equals "default" are deliberately
     * skipped (returns false).
     */
    bool initPlugin(WPlugin *plugin, InitDataProc proc = [](WMessage &) {});

    /**
     * @brief Unloads a plugin (the instance is kept in the registry).
     * @return true if the plugin was unloaded.
     */
    bool unloadPlugin(WPlugin *plugin);

    /**
     * @brief Unloads a plugin without removing it from the registry.
     * @return true if the plugin was unloaded.
     */
    bool hotUnloadPlugin(WPlugin *plugin);

    /// Unloads every plugin and drops the successfully unloaded ones.
    void unloadAllPlugins();

    /// UUID of the first plugin named @p name, or a null UUID if not found.
    QUuid getPluginByName(const QString &name) const;

    /// Plugin instance for @p id, or null if not registered.
    WPlugin *getPluginById(const QUuid &id) const;

    /// Raw pointers to all registered plugins (ownership stays with callers).
    QVector<WPlugin *> allPluginsInst() const;

    /**
     * @brief Stores a metadata value, returning the value actually kept.
     * @param id    Plugin instance UUID.
     * @param key   Metadata key.
     * @param value Requested value.
     * @return For Plugin::Name, a unique name derived from @p value; for any
     *         other key, @p value unchanged.
     */
    QVariant setPluginData(const QUuid &id, const QString &key,
                           const QVariant &value);

    /// UUIDs of all plugins whose @p key metadata equals @p value.
    QVector<QUuid> getPluginByAttr(const QString &key,
                                   const QVariant &value) const;

    /// UUIDs of all registered plugins.
    QVector<QUuid> allPluginsId() const;

public slots:
    /**
     * @brief Delivers @p msg to the plugin named WMessage::dest.
     * @return true if the target plugin exists and has a valid interface.
     */
    bool sendMsg(we::WMessage &msg);

private:
    QString makeUniquePluginName(const QString &baseName,
                                 const QUuid &excludeId) const;

private:
    QScopedPointer<WPluginManagerPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WPluginManager)
};

} // namespace we

Q_DECLARE_METATYPE(we::WPluginManager *)

#endif // PLUGINMANAGER_H