/**
 * @file WEvent.h
 * @brief Represents an event on the framework’s publish‑subscribe bus.
 *
 * WEvent wraps a topic, a message, and metadata (sender, timestamp,
 * correlation ID) used by WWidgetManager and other event‑driven components.
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
#ifndef WEVENT_H
#define WEVENT_H

#include <QDateTime>
#include <QString>
#include <QVariant>

#include "WECore/plugin/wpluginmessage.h"

namespace we {

/**
 * @brief An event transmitted over the event bus.
 *
 * `topic` is mandatory and selects the subscribers, `msg` carries the
 * optional payload, `timestamp` is stamped automatically at construction,
 * and `correlationId` links a reply event back to its originating request.
 */
struct WEvent {
    QString topic;         ///< Event topic (e.g., "user.login").
    WMessage msg;          ///< Arbitrary message payload.
    QString sender;        ///< Identifier of the publisher.
    qint64 timestamp;      ///< Millisecond timestamp (UTC).
    QString correlationId; ///< Optional correlation ID for request/response.

    /// Default constructor – timestamp is set to now.
    WEvent() : timestamp(QDateTime::currentMSecsSinceEpoch()) {}

    /// Constructs an event with a topic, optional message, and sender.
    explicit WEvent(const QString &t, const WMessage &m = {},
                    const QString &s = {})
        : topic(t), msg(m), sender(s),
        timestamp(QDateTime::currentMSecsSinceEpoch()) {}

    /// Returns msg.map[key] cast to T, or a default‑constructed T if it is missing or incompatible.
    template <typename T> T dataValue(const QString &key) const {
        return qvariant_cast<T>(msg.map.value(key));
    }
};

} // namespace we

Q_DECLARE_METATYPE(we::WEvent)

#endif // WEVENT_H