/**
 * @file wpluginstatemachine.h
 * @brief Plugin state machine for managing plugin lifecycle states.
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
#ifndef WPLUGINSTATEMACHINE_H
#define WPLUGINSTATEMACHINE_H

#include <QObject>
#include "WECore/def/wedef.h"

namespace we {

class WPlugin;

/// Lifecycle states of a plugin.
enum class PluginState {
    Unloaded,  ///< No backend loaded; the initial state.
    Loading,   ///< A load is in progress.
    Loaded,    ///< Backend loaded and initialised.
    Unloading, ///< An unload is in progress.
    Error,     ///< The last load/unload attempt failed.
    Disabled   ///< Plugin disabled by the user or configuration.
};

/**
 * @brief Synchronous state machine guarding plugin lifecycle transitions.
 *
 * Transitions follow a fixed table; an illegal transition leaves the current
 * state untouched and is reported through errorOccurred(). The implementation
 * is synchronous, so the goTo* signals exist only for API compatibility and
 * are never emitted.
 */
class WE_EXPORT WPluginStateMachine : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(WPluginStateMachine)

public:
    /**
     * @brief Constructs the machine in the Unloaded state.
     * @param parent Owning WPlugin. The QObject parent is set to
     *               parent->parent() (the plugin manager) so the machine lives
     *               as long as the manager; @p parent may be null.
     */
    explicit WPluginStateMachine(WPlugin *parent = nullptr);

    ~WPluginStateMachine() override;

    PluginState currentState() const;

    /// Attempts a transition; false (state unchanged) if it is not allowed.
    bool transitionTo(PluginState newState);

    /// Localised, user-facing name of @p state, e.g. for labels in the UI.
    static QString stateToString(PluginState state);

    /**
     * @brief Parses a state name.
     * @return The matching state, or Unloaded if unrecognised.
     * @note Only the non-localised English names are recognised, so this does
     *       not round-trip the output of stateToString() in translated builds.
     */
    static PluginState stringToState(const QString &stateStr);

signals:
    /// Emitted after a successful transition.
    void stateChanged(we::PluginState oldState, PluginState newState);

    /// Emitted when a transition is rejected; @p error is already localised.
    void errorOccurred(const QString &error, we::PluginState state);

    // Never emitted (synchronous implementation); kept for API compatibility.
    void goToLoading();
    void goToLoaded();
    void goToUnloading();
    void goToUnloaded();
    void goToError();
    void goToDisabled();

private:
    void setCurrentState(PluginState newState);

private:
    PluginState m_currentState;
    WPlugin *m_parent;
};

} // namespace we

#endif // WPLUGINSTATEMACHINE_H