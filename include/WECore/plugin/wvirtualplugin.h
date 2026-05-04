/**
 * @file wvirtualplugin.h
 * @brief Virtual plugin that bridges an external executable through an event
 * bus.
 *
 * WVirtualPlugin implements WPluginInterface by launching an external program
 * in response to events published on a global event bus. It is typically used
 * to integrate non‑Qt plugins (e.g., .exe, .bat) into the system.
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
#ifndef WVIRTUALPLUGIN_H
#define WVIRTUALPLUGIN_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "WECore/def/wedef.h"
#include "WECore/plugin/wplugininterface.h"

namespace we {

class WVirtualPluginPrivate;

/**
 * @brief A virtual plugin that relays event bus messages to an external
 * executable.
 *
 * When an event matching the plugin's subscribed topics is published on the
 * global widget manager / event bus, the virtual plugin translates it into
 * command‑line arguments and launches the configured executable. It also
 * supports an optional initialisation command and admin privileges.
 *
 * @note This class inherits both QObject (for ownership / signal‑slot) and
 *       WPluginInterface (for lifecycle management).
 */
class WE_EXPORT WVirtualPlugin : public QObject, public WPluginInterface {
    Q_OBJECT
    Q_DISABLE_COPY(WVirtualPlugin)
    Q_INTERFACES(WPluginInterface)
public:
    /**
   * @brief Constructs a virtual plugin.
   * @param parent Optional parent QObject.
   */
    explicit WVirtualPlugin(QObject *parent = nullptr);

    /// Destroys the plugin and unsubscribes from all events.
    ~WVirtualPlugin() override;

    /**
   * @brief Initialises the plugin: selects topics, subscribes, and may launch
   * the executable.
   * @param msg Initialisation message (currently unused).
   * @return @c true on success.
   */
    bool init(WMessage &msg) override;

    /**
   * @brief Receives a WMessage from the plugin manager.
   *
   * This implementation ignores the message; all communication is performed
   * through the event bus.
   *
   * @param msg The incoming message (unused).
   */
    void recMsg(WMessage &msg) override;

    /**
   * @brief Deinitialises the plugin: unsubscribes and cleans up.
   * @param msg Deinitialisation message (currently unused).
   * @return @c true on success.
   */
    bool deinit(WMessage &msg) override;

    /**
   * @brief Sets the path to the executable file.
   * @param filePath Absolute or relative path to the executable.
   */
    void setFile(const QString &filePath);

    /**
   * @brief Returns the configured executable path.
   */
    QString getFilePath() const;

    /**
   * @brief Associates this virtual plugin with a WPlugin metadata object.
   * @param plugin A pointer to the owning WPlugin instance.
   */
    void setPlugin(WPlugin *plugin);

    /**
   * @brief Explicitly sets the list of event topics to listen for.
   * @param topics List of topic strings (e.g., "plugin.login").
   *
   * If not set manually, topics are derived from the plugin’s metadata.
   */
    void setTopics(const QStringList &topics);

    /**
   * @brief Returns the current list of subscribed topics.
   */
    QStringList getTopics() const;

private:
    /// Called when an event matching a subscribed topic is published.
    void onEventReceived(const WEvent &event);

    /// Subscribes to all topics in the internal list.
    void subscribeTopics();

    QScopedPointer<WVirtualPluginPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WVirtualPlugin)
};

} // namespace we

#endif // WVIRTUALPLUGIN_H