/**
 * @file wpluginmessage.h
 * @brief Defines the WMessage struct used for inter-plugin and manager
 * communication.
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
#ifndef WPLUGINMESSAGE_H
#define WPLUGINMESSAGE_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QVariant>

namespace we {

/**
 * @brief Lightweight message exchanged between plugins and the manager.
 *
 * @note Registered with Q_DECLARE_METATYPE, so it can be stored in a QVariant
 *       and sent through queued connections.
 */
struct WMessage
{
    QString from;                ///< Sender identifier (plugin name or ID).
    QString dest;                ///< Recipient plugin name; the manager routes by this.
    QString command;             ///< Action the recipient should perform.
    int type = 0;                ///< Application-defined message category.
    QMap<QString, QVariant> map; ///< Payload data as key-value pairs.
    QObject *object = nullptr;   ///< Optional associated QObject.
};

} // namespace we

Q_DECLARE_METATYPE(we::WMessage)

#endif // WPLUGINMESSAGE_H