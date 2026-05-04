/**
 * @file wplugininterface.h
 * @brief Abstract plugin interface definition.
 *
 * Every plugin that is loaded by the WPluginManager must implement
 * this interface and register it with Q_INTERFACES().
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
#ifndef WPLUGININTERFACE_H
#define WPLUGININTERFACE_H

#include <QtPlugin>

#include "WECore/def/wedef.h"

/**
 * @brief The interface that all plugins must implement.
 *
 * Provides the lifecycle hooks init(), recMsg() and deinit() that
 * the plugin manager calls during loading, messaging and unloading.
 *
 * @note Implementors must add `Q_INTERFACES(WPluginInterface)` to their
 *       plugin class and export it using `Q_PLUGIN_METADATA`.
 */
class WPluginInterface
{
    Q_DISABLE_COPY(WPluginInterface)

public:
    /// Default constructor.
    WPluginInterface() = default;

    /// Virtual destructor for safe polymorphic deletion.
    virtual ~WPluginInterface() = default;

    /**
     * @brief Called once after the plugin has been loaded.
     * @param msg  Message object that can carry initialisation data.
     * @return @c true if the plugin initialised successfully;
     *         otherwise @c false, which will cause unloading.
     */
    virtual bool init(we::WMessage &msg) = 0;

    /**
     * @brief Receives a runtime message from the plugin manager.
     * @param msg The message to process (may be modified by the plugin).
     *
     * This function is the primary communication channel and may be
     * called at any time while the plugin is loaded.
     */
    virtual void recMsg(we::WMessage &msg) = 0;

    /**
     * @brief Called when the plugin is about to be unloaded.
     * @param msg  Message object for passing shutdown status back.
     * @return @c true if the plugin successfully cleaned up;
     *         @c false if the plugin cannot be unloaded yet.
     */
    virtual bool deinit(we::WMessage &msg) = 0;
};

/// Unique identifier for the WPluginInterface.
/// Used by Qt's meta‑object system to recognise the interface.
#define WPluginInterface_iid "QPlugins.WPluginManager.WPluginInterface"

Q_DECLARE_INTERFACE(WPluginInterface, WPluginInterface_iid)

#endif // WPLUGININTERFACE_H