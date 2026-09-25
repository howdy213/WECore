/**
 * @file wvirtualplugin.h
 * @brief Bridges an external executable into the plugin system through the
 * event bus.
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
 * @brief WPluginInterface implementation that launches an external program.
 *
 * Events published on the global event bus (the widget manager) are matched
 * against the subscribed topics and turned into a command line for the
 * configured executable, optionally elevated with administrator rights. Used
 * to integrate non-Qt backends such as .exe/.bat files.
 *
 * @note Inherits QObject (ownership / signal-slot) and WPluginInterface
 *       (lifecycle).
 */
class WE_EXPORT WVirtualPlugin : public QObject, public WPluginInterface {
    Q_OBJECT
    Q_DISABLE_COPY(WVirtualPlugin)
    Q_INTERFACES(WPluginInterface)
public:
    explicit WVirtualPlugin(QObject *parent = nullptr);

    /// Unsubscribes from the event bus on destruction.
    ~WVirtualPlugin() override;

    /**
     * @brief Resolves the event bus and subscribes to the configured topics.
     * @param msg Unused.
     * @return false if no WPlugin was associated or no event bus is available.
     *
     * Fails unless setPlugin() was called beforehand.
     */
    bool init(WMessage &msg) override;

    /// No-op: communication happens through the event bus, not this channel.
    void recMsg(WMessage &msg) override;

    /// Unsubscribes from the event bus.
    bool deinit(WMessage &msg) override;

    /// Path of the executable to launch. Not validated here.
    void setFile(const QString &filePath);
    QString getFilePath() const;

    /// Associates the owning WPlugin; required before init().
    void setPlugin(WPlugin *plugin);

    /// Overrides the topic list; otherwise init() derives
    /// "VirtualPlugin.<name>" from the plugin metadata.
    void setTopics(const QStringList &topics);
    QStringList getTopics() const;

private:
    /// Launches the executable for an event received on a subscribed topic.
    void onEventReceived(const WEvent &event);

    /// Subscribes onEventReceived() to every topic in the list.
    void subscribeTopics();

    QScopedPointer<WVirtualPluginPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WVirtualPlugin)
};

} // namespace we

#endif // WVIRTUALPLUGIN_H