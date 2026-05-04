/**
 * @file wpluginmessage.h
 * @brief Defines the WMessage struct used for inter‑plugin and manager communication.
 *
 * WMessage is a lightweight message object that carries a command, a source
 * and destination identifier, a payload map of key‑value pairs, and an
 * optional sender/receiver QObject. It is registered with the Qt meta‑type
 * system so that it can be used inside signal/slot connections and QVariant.
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
#ifndef WPLUGINMESSAGE_H
#define WPLUGINMESSAGE_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QVariant>

namespace we {

/**
 * @brief Represents a message exchanged between plugins and the plugin manager.
 *
 * The struct contains:
 * - A sender identifier (`from`), typically a plugin name or ID.
 * - A destination identifier (`dest`), which is used by the manager to route the message.
 * - A `command` string that describes the action to perform.
 * - A numerical `type` that can further categorize the message.
 * - A key‑value `map` for arbitrary payload data.
 * - An optional `object` pointer to a QObject sender/receiver.
 *
 * @note This struct is registered with Q_DECLARE_METATYPE so it can be stored
 *       in QVariant and transmitted across queued connections.
 */
struct WMessage
{
    QString from;                   ///< Identifier of the sender.
    QString dest;                   ///< Identifier of the intended recipient.
    QString command;                ///< Action the recipient should perform.
    int type = 0;                   ///< Numeric message type, application‑specific.
    QMap<QString, QVariant> map;    ///< Payload data as key‑value pairs.
    QObject *object = nullptr;      ///< Optional QObject associated with the message.
};

} // namespace we

Q_DECLARE_METATYPE(we::WMessage)

#endif // WPLUGINMESSAGE_H