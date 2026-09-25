/**
 * @file wplugininterface.h
 * @brief Abstract plugin interface definition.
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
#ifndef WPLUGININTERFACE_H
#define WPLUGININTERFACE_H

#include <QtPlugin>

#include "WECore/def/wedef.h"

/**
 * @brief The interface that every plugin must implement.
 *
 * Provides the lifecycle hooks init(), recMsg() and deinit() that the plugin
 * manager calls while loading, messaging and unloading.
 *
 * @note Implementations must add `Q_INTERFACES(WPluginInterface)` and export
 *       the class with `Q_PLUGIN_METADATA`.
 */
class WPluginInterface
{
    Q_DISABLE_COPY(WPluginInterface)

public:
    WPluginInterface() = default;

    virtual ~WPluginInterface() = default;

    /**
     * @brief Called once after the plugin has been loaded.
     * @param msg Message carrying initialisation data.
     * @return true if the plugin initialised successfully; false makes the
     *         manager unload it again.
     */
    virtual bool init(we::WMessage &msg) = 0;

    /**
     * @brief Receives a runtime message from the plugin manager.
     * @param msg Message to process; may be modified by the plugin.
     *
     * The primary communication channel; may be called at any time while the
     * plugin is loaded.
     */
    virtual void recMsg(we::WMessage &msg) = 0;

    /**
     * @brief Called when the plugin is about to be unloaded.
     * @param msg Message for passing the shutdown status back.
     * @return true if cleanup succeeded; false if the plugin cannot be
     *         unloaded yet.
     */
    virtual bool deinit(we::WMessage &msg) = 0;
};

/// Interface IID; must match Q_DECLARE_INTERFACE and Q_PLUGIN_METADATA.
#define WPluginInterface_iid "QPlugins.WPluginManager.WPluginInterface"

Q_DECLARE_INTERFACE(WPluginInterface, WPluginInterface_iid)

#endif // WPLUGININTERFACE_H