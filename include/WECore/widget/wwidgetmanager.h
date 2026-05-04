/**
 * @file wwidgetmanager.h
 * @brief Event bus and widget registry for the plugin framework.
 *
 * WWidgetManager serves as both a widget metadata store and a
 * publish‑subscribe event bus. It allows registration of widgets
 * (with associated plugin interfaces), supports pattern‑based
 * topic subscriptions, and provides a request/response mechanism
 * with a timeout.
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
#ifndef WWIDGETMANAGER_H
#define WWIDGETMANAGER_H

#include <QFuture>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QUuid>
#include <QVariant>
#include <QVector>
#include <functional>

#include "WECore/def/wedef.h"
#include "WECore/metadata/WEvent.h"
#include "WECore/plugin/wplugininterface.h"

namespace we {

using SubscribeFunc = std::function<void(const WEvent &)>;

class WWidgetManagerPrivate;
class WEBase;

/**
 * @brief Central widget manager and event bus.
 *
 * Each widget is registered together with a plugin interface and a unique ID.
 * The event bus allows components to publish events on named topics; subscribers
 * using glob‑style patterns (*, **) are invoked when a matching event is published.
 *
 * A built‑in request/response mechanism uses temporary reply topics and a timeout.
 */
class WE_EXPORT WWidgetManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WWidgetManager)

public:
    /**
     * @brief Constructs a widget manager.
     * @param base  Pointer to the owning WEBase instance (may be nullptr).
     */
    explicit WWidgetManager(WEBase *base = nullptr);

    /// Destroys the manager, cleaning up subscriptions and widget data.
    ~WWidgetManager() override;

    // ---------- Widget registration ----------
    /**
     * @brief Registers a widget with a unique ID and its owning plugin interface.
     * @param id      The widget’s unique identifier.
     * @param widget  The QObject representing the widget.
     * @param plugin  The plugin interface that hosts the widget.
     * @return @c true on success; @c false if the widget or ID is already registered.
     */
    bool addWidget(QUuid id, QObject *widget, WPluginInterface *plugin);

    /**
     * @brief Retrieves a widget by its UUID.
     * @param id  The widget UUID.
     * @return A pointer to the widget, or @c nullptr if not found.
     */
    QObject *getWidget(QUuid id);

    /**
     * @brief Returns all registered widgets.
     */
    QVector<QObject *> getWidgets();

    /**
     * @brief Finds widgets whose attribute @p key matches @p value.
     * @param key    Attribute key.
     * @param value  Desired attribute value.
     * @return A vector of matching widget pointers.
     */
    QVector<QObject *> getWidget(const QString &key, const QVariant &value);

    /**
     * @brief Returns the UUID of a registered widget.
     * @param widget  A widget previously added via addWidget().
     * @return The widget’s UUID, or a null UUID if not found.
     */
    QUuid getUuid(QObject *widget);

    /**
     * @brief Reads a widget attribute.
     * @param widget  The widget.
     * @param key     Attribute key.
     * @return The attribute value, or an invalid QVariant if not found.
     */
    QVariant getAttr(QObject *widget, const QString &key);

    /**
     * @brief Sets a widget attribute, with automatic name deduplication for "Name".
     * @param widget  The widget.
     * @param key     Attribute key.
     * @param value   New value.
     * @return @c true on success; @c false if the widget is not registered.
     */
    bool setAttr(QObject *widget, const QString &key, const QVariant &value);

    /// Calls initWidget() on all registered widgets (typically once after loading).
    void initWidget();

    // ---------- Event bus: subscribe ----------
    /**
     * @brief Subscribes to a topic pattern using a member function pointer.
     * @param pattern  Topic pattern (e.g., "user.*", "login").
     * @param receiver The object that will receive the event.
     * @param slot     A member function with the signature `void(const WEvent &)`.
     * @return @c true on success.
     *
     * This is a templated convenience overload that wraps the member function
     * into a SubscribeFunc.
     */
    template <typename Func>
    bool subscribe(const QString &pattern, QObject *receiver, Func slot);

    /**
     * @brief Subscribes to a topic pattern using a generic callback.
     * @param pattern  Topic pattern.
     * @param context  The subscriber QObject (used for lifetime tracking).
     * @param callback A callable with the signature `void(const WEvent &)`.
     * @return @c true on success.
     */
    bool subscribe(const QString &pattern, QObject *context,
                   SubscribeFunc callback);

    /// Removes all subscriptions associated with @p receiver.
    void unsubscribeAll(QObject *receiver);

    /// Removes subscriptions matching @p receiver and @p pattern.
    void unsubscribe(QObject *receiver, const QString &pattern);

    // ---------- Event bus: publish ----------
    /**
     * @brief Publishes an event (asynchronous – thread‑safe).
     *
     * If called from a non‑GUI thread, the event is queued to the manager’s
     * owning thread.
     * @param event  The event to publish.
     */
    void publish(const WEvent &event);

    /**
     * @brief Publishes an event synchronously (immediate delivery).
     * @param event  The event to publish.
     */
    void publishSync(const WEvent &event);

    // ---------- Request / Response ----------
    /**
     * @brief Sends a request on @p topic and awaits a reply.
     * @param pattern    The request topic.
     * @param data       Payload to send.
     * @param timeoutMs  Timeout in milliseconds (default 5000).
     * @return A QFuture that will contain the reply data or an exception on timeout.
     */
    QFuture<QVariant> request(const QString &pattern, const QVariant &data,
                              int timeoutMs = 5000);

signals:
    /// Emitted after every event dispatch (can be used for logging or auditing).
    void eventDispatched(const we::WEvent &event);

private:
    QScopedPointer<WWidgetManagerPrivate> d_ptr;

    /// Invokes a callback in the receiver's thread.
    void invokeCallback(const SubscribeFunc &cb, QObject *receiver,
                        const WEvent &event);

    /// Cleans up all subscriptions for a destroyed receiver.
    void onReceiverDestroyed(QObject *receiver);

    /**
     * @brief Adjusts a value before storing it as a widget attribute.
     *        Currently handles automatic name deduplication.
     * @param widget  The widget whose attribute is being set.
     * @param key     Attribute key.
     * @param value  Desired value.
     * @return The adjusted value (e.g., a unique name).
     */
    QVariant changeVariant(QObject *widget, const QString &key, const QVariant &value);

    /**
     * @brief Compiles a glob‑style topic pattern into a QRegularExpression.
     * @param pattern  Pattern string (e.g., "user.*.name").
     * @return A QRegularExpression anchored to match the whole topic.
     */
    static QRegularExpression compilePattern(const QString &pattern);
};

// ---------- Template implementation ----------
template <typename Func>
bool WWidgetManager::subscribe(const QString &pattern, QObject *receiver,
                               Func slot)
{
    static_assert(std::is_member_function_pointer_v<Func>,
                  "Must be a member function pointer");
    auto callback = [receiver, slot](const WEvent &event) {
        (receiver->*slot)(event);
    };
    return subscribe(pattern, receiver, std::move(callback));
}

} // namespace we

#endif // WWIDGETMANAGER_H