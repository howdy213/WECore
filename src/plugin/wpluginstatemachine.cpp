/**
 * @file wpluginstatemachine.cpp
 * @brief Implementation of WPluginStateMachine (synchronous version).
 *
 * Contains the implementation of the plugin state machine for managing
 * plugin lifecycle states.
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

#include "WECore/plugin/wpluginstatemachine.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/plugin/wplugin.h"

#include <QCoreApplication>
#include <QDebug>

namespace we {

// Static utility functions
QString WPluginStateMachine::stateToString(PluginState state) {
    switch (state) {
    case PluginState::Unloaded:
        return "Unloaded";
    case PluginState::Loading:
        return "Loading";
    case PluginState::Loaded:
        return "Loaded";
    case PluginState::Unloading:
        return "Unloading";
    case PluginState::Error:
        return "Error";
    case PluginState::Disabled:
        return "Disabled";
    default:
        return "Unknown";
    }
}

PluginState WPluginStateMachine::stringToState(const QString &stateStr) {
    QString lowerState = stateStr.toLower();
    if (lowerState == "unloaded") return PluginState::Unloaded;
    if (lowerState == "loading") return PluginState::Loading;
    if (lowerState == "loaded") return PluginState::Loaded;
    if (lowerState == "unloading") return PluginState::Unloading;
    if (lowerState == "error") return PluginState::Error;
    if (lowerState == "disabled") return PluginState::Disabled;
    return PluginState::Unloaded;
}

// WPluginStateMachine implementation
WPluginStateMachine::WPluginStateMachine(WPlugin *parent)
    : QObject(parent->parent())
    , m_currentState(PluginState::Unloaded)
    , m_parent(parent) {
}

WPluginStateMachine::~WPluginStateMachine() {
    // Nothing to clean up
}

PluginState WPluginStateMachine::currentState() const {
    return m_currentState;
}

bool WPluginStateMachine::transitionTo(PluginState newState) {
    if (m_currentState == newState) {
        return true;
    }

    // Validate state transition
    bool validTransition = false;
    switch (m_currentState) {
    case PluginState::Unloaded:
        validTransition = (newState == PluginState::Loading ||
                           newState == PluginState::Disabled);
        break;
    case PluginState::Loading:
        validTransition = (newState == PluginState::Loaded ||
                           newState == PluginState::Error);
        break;
    case PluginState::Loaded:
        validTransition = (newState == PluginState::Unloading ||
                           newState == PluginState::Error);
        break;
    case PluginState::Unloading:
        validTransition = (newState == PluginState::Unloaded);
        break;
    case PluginState::Error:
        validTransition = (newState == PluginState::Unloaded ||
                           newState == PluginState::Loading);
        break;
    case PluginState::Disabled:
        validTransition = (newState == PluginState::Loading ||
                           newState == PluginState::Unloaded);
        break;
    }

    if (!validTransition) {
        QString errorMsg = QString("Invalid transition from %1 to %2")
                               .arg(stateToString(m_currentState), stateToString(newState));
        qWarning() << "WPluginStateMachine::transitionTo:" << errorMsg;
        emit errorOccurred(errorMsg, m_currentState);
        return false;
    }

    // Perform synchronous state change
    PluginState oldState = m_currentState;
    m_currentState = newState;
    qDebug() << "WPluginStateMachine::transitionTo: State changed from"
             << stateToString(oldState) << "to" << stateToString(newState);
    emit stateChanged(oldState, newState);

    return true;
}

} // namespace we