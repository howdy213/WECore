/**
 * @file wpluginmanager.h
 * @brief Plugin manager – loads, tracks, and communicates with plugins.
 *
 * WPluginManager is the central registry for all plugins. It assigns
 * unique IDs, handles the lifecycle (load/unload/init), stores persistent
 * metadata, and routes inter‑plugin messages.
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
 * @brief Callback type for providing initialisation data to a plugin.
 * @param msg  Mutable WMessage that will be passed to WPluginInterface::init().
 */
using InitDataProc = std::function<void(WMessage &msg)>;

class WPluginManagerPrivate;

/**
 * @brief Manages the complete lifecycle of WPlugin instances.
 *
 * WPluginManager is a QObject that can be used with Qt’s signal/slot
 * mechanism. It acts as the owner of all plugins, provides persistence
 * for metadata, and routes messages between plugins.
 *
 * @note The class is non‑copyable. Use a pointer if it must be stored
 *       inside a QVariant (`Q_DECLARE_METATYPE(we::WPluginManager *)`).
 */
class WE_EXPORT WPluginManager : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(WPluginManager)

public:
    /// Constructs an empty plugin manager.
    explicit WPluginManager(QObject *parent = nullptr);

    /// Destroys the manager. All plugins are unloaded (if not already done).
    ~WPluginManager() override;

    /**
   * @brief Registers a plugin with the manager.
   * @param plugin  A plugin instance created by the caller.
   * @return @c true on success.
   *
   * The manager assigns a unique ID and ensures the plugin’s name is unique.
   */
    bool addPlugin(WPlugin *plugin);

    /**
   * @brief Loads the plugin’s backend (DLL or EXE).
   * @param plugin  The plugin to load.
   * @return @c true if the plugin was successfully loaded or was already
   * loaded.
   */
    bool loadPlugin(WPlugin *plugin);

    /**
   * @brief Initialises a loaded plugin.
   * @param plugin  The plugin to initialise.
   * @param proc    Optional callback that can modify the initialisation
   * message.
   * @return @c true on success.
   *
   * If the plugin is an executable and its metadata key "Init" equals
   * "default", initialisation is skipped (returns @c false).
   */
    bool initPlugin(WPlugin *plugin, InitDataProc proc = [](WMessage &) {});

    /**
   * @brief Unloads a plugin and removes it from the registry.
   * @param plugin  The plugin to unload.
   * @return @c true if the plugin was successfully unloaded and removed.
   */
    bool unloadPlugin(WPlugin *plugin);

    /// Unloads every registered plugin.
    void unloadAllPlugins();

    /**
   * @brief Finds a plugin’s UUID by its name.
   * @param name  The plugin name (case‑sensitive).
   * @return The plugin’s UUID, or a null UUID if not found.
   */
    QUuid getPluginByName(const QString &name) const;

    /**
   * @brief Returns the plugin instance for a given UUID.
   * @param id  The plugin’s unique ID.
   * @return A pointer to the plugin, or @c nullptr if not found.
   */
    WPlugin *getPluginById(const QUuid &id) const;

    /**
   * @brief Returns all currently registered plugin instances.
   * @return A vector of raw WPlugin pointers.
   */
    QVector<WPlugin *> allPluginsInst() const;

    /**
   * @brief Stores a metadata value in the manager’s persistent storage.
   * @param id     The plugin’s UUID.
   * @param key    The metadata key.
   * @param value  The new value.
   * @return The value that should actually be stored (e.g. a name
   *         that has been made unique).
   *
   * If the key is `Plugin::Name`, the manager ensures the resulting
   * name is unique by appending '#' characters if necessary.
   */
    QVariant setPluginData(const QUuid &id, const QString &key,
                           const QVariant &value);

    /**
   * @brief Finds plugins whose metadata matches a given key/value pair.
   * @param key    Metadata key.
   * @param value  Desired value.
   * @return UUIDs of all matching plugins.
   */
    QVector<QUuid> getPluginByAttr(const QString &key,
                                   const QVariant &value) const;

    /**
   * @brief Returns the UUIDs of all registered plugins.
   * @return A vector of UUIDs.
   */
    QVector<QUuid> allPluginsId() const;

    /**
   * @brief Retrieves the UUID assigned to a given plugin instance.
   * @param plugin  A registered plugin.
   * @return The plugin’s UUID, or a null UUID if not found.
   */
    QUuid getUuid(const WPlugin *plugin) const;

public slots:
    /**
   * @brief Delivers a message to the plugin whose name matches WMessage::dest.
   * @param msg  The message to deliver.
   * @return @c true if the target plugin was found and could process the
   * message.
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

// Allow storage of the pointer inside QVariant.
Q_DECLARE_METATYPE(we::WPluginManager *)

#endif // PLUGINMANAGER_H