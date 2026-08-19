/**
 * @file wpluginstatemachine.h
 * @brief Plugin state machine for managing plugin lifecycle states.
 *
 * Provides a state machine implementation to manage the different states
 * a plugin can be in: unloaded, loading, loaded, unloading, error, etc.
 *
 * @author howdy213
 * @date 2026-08-19
 * @version 1.0.0
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
#ifndef WPLUGINSTATEMACHINE_H
#define WPLUGINSTATEMACHINE_H

#include <QObject>
#include "WECore/def/wedef.h"

namespace we {

class WPlugin;

/**
 * @brief Plugin state enumeration.
 */
enum class PluginState {
    Unloaded,    ///< Plugin is not loaded
    Loading,     ///< Plugin is being loaded
    Loaded,      ///< Plugin is successfully loaded and initialized
    Unloading,   ///< Plugin is being unloaded
    Error,       ///< Plugin is in error state
    Disabled     ///< Plugin is disabled
};

/**
 * @brief Plugin state machine for managing plugin lifecycle.
 *
 * This class provides a synchronous state machine implementation to manage the
 * different states a plugin can be in during its lifecycle.
 */
class WE_EXPORT WPluginStateMachine : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(WPluginStateMachine)

public:
    /**
     * @brief Constructs a plugin state machine.
     * @param parent The parent plugin instance.
     */
    explicit WPluginStateMachine(WPlugin *parent = nullptr);

    /**
     * @brief Destroys the plugin state machine.
     */
    ~WPluginStateMachine() override;

    /**
     * @brief Gets the current state of the plugin.
     * @return The current plugin state.
     */
    PluginState currentState() const;

    /**
     * @brief Attempts to transition to a new state.
     * @param newState The target state.
     * @return true if transition was successful, false if invalid.
     */
    bool transitionTo(PluginState newState);

    /**
     * @brief Gets the state as a string representation.
     * @param state The state to convert.
     * @return String representation of the state.
     */
    static QString stateToString(PluginState state);

    /**
     * @brief Gets the state from a string representation.
     * @param stateStr The string representation.
     * @return The corresponding state.
     */
    static PluginState stringToState(const QString &stateStr);

signals:
    /**
     * @brief Signal emitted when the plugin state changes.
     * @param oldState The previous state.
     * @param newState The new state.
     */
    void stateChanged(we::PluginState oldState, PluginState newState);

    /**
     * @brief Signal emitted when an error occurs in the state machine.
     * @param error The error message.
     * @param state The state where the error occurred.
     */
    void errorOccurred(const QString &error, we::PluginState state);

    // Signals used for internal transition triggering (kept for API compatibility, but not emitted in synchronous mode)
    void goToLoading();
    void goToLoaded();
    void goToUnloading();
    void goToUnloaded();
    void goToError();
    void goToDisabled();

private:
    /**
     * @brief Updates the current state and emits stateChanged signal.
     * @param newState The new state to set.
     */
    void setCurrentState(PluginState newState);

private:
    PluginState m_currentState;
    WPlugin *m_parent;
};

} // namespace we

#endif // WPLUGINSTATEMACHINE_H