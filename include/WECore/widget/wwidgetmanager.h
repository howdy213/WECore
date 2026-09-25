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
 * @brief Central widget registry and publish‑subscribe event bus.
 *
 * Each registered widget carries a unique ID and its owning plugin interface,
 * so it can later be looked up by ID, by object, or by attribute.
 *
 * The event bus lets components publish WEvent on named topics; subscribers
 * may register glob patterns (`*` matches one segment, `**` matches any) and
 * are invoked when a matching topic is published.
 *
 * A built‑in request/response mechanism pairs a reply with its request through
 * a generated correlation ID plus a timeout.
 */
class WE_EXPORT WWidgetManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WWidgetManager)

public:
    /// @p base is kept for plugin/widget lookups and may be null.
    explicit WWidgetManager(WEBase *base = nullptr);

    ~WWidgetManager() override;

    // ---------- Widget registration ----------
    /// Registers @p widget under @p id, owned by @p plugin; returns @c false if the widget is already registered or @p id is taken.
    bool addWidget(QUuid id, QObject *widget, WPluginInterface *plugin);

    /// Widget registered under @p id, or @c nullptr.
    QObject *getWidget(QUuid id);

    /// All registered widgets.
    QVector<QObject *> getWidgets();

    /// Widgets whose attribute @p key equals @p value.
    QVector<QObject *> getWidget(const QString &key, const QVariant &value);

    /// UUID of @p widget, or a null UUID if it is not registered.
    QUuid getUuid(QObject *widget);

    /// Value of attribute @p key on @p widget, or an invalid QVariant if absent.
    QVariant getAttr(QObject *widget, const QString &key);

    /// Sets attribute @p key on @p widget (with "Name" deduplication); returns @c false if @p widget is unregistered.
    bool setAttr(QObject *widget, const QString &key, const QVariant &value);

    /// Calls initWidget() on every registered widget; typically invoked once after loading.
    void initWidget();

    // ---------- Event bus: subscribe ----------
    /**
     * @brief Subscribes @p receiver to @p pattern using one of its member functions.
     *
     * @p slot must take `const WEvent &`. The subscription stays alive until
     * @p receiver is destroyed or is removed explicitly; the callback is run in
     * @p receiver's thread.
     */
    template <typename Func>
    bool subscribe(const QString &pattern, QObject *receiver, Func slot);

    /**
     * @brief Subscribes @p context to @p pattern with an arbitrary callback.
     *
     * @p callback must take `const WEvent &`; the subscription is dropped
     * automatically when @p context is destroyed.
     */
    bool subscribe(const QString &pattern, QObject *context,
                   SubscribeFunc callback);

    /// Removes every subscription of @p receiver (and any whose receiver is already gone).
    void unsubscribeAll(QObject *receiver);

    /// Removes all subscriptions of @p receiver for @p pattern.
    void unsubscribe(QObject *receiver, const QString &pattern);

    // ---------- Event bus: publish ----------
    /**
     * @brief Publishes @p event to matching subscribers asynchronously.
     *
     * Thread‑safe: when called from a thread other than the manager's, dispatch
     * is queued onto the manager's thread. Each callback runs in its receiver's
     * thread.
     */
    void publish(const WEvent &event);

    /**
     * @brief Publishes @p event to matching subscribers immediately.
     *
     * Callbacks run synchronously in the calling thread, so re‑entrancy is
     * possible. Unlike publish(), there is no thread hop and eventDispatched is
     * not emitted.
     */
    void publishSync(const WEvent &event);

    // ---------- Request / Response ----------
    /**
     * @brief Sends a request on @p pattern and returns a future carrying the reply.
     *
     * The reply is matched by a generated correlation ID; if none arrives within
     * @p timeoutMs, the future reports a "Request timed out" exception.
     */
    QFuture<QVariant> request(const QString &pattern, const QVariant &data,
                              int timeoutMs = 5000);

signals:
    /// Emitted by publish() after dispatch; not emitted by publishSync().
    void eventDispatched(const we::WEvent &event);

private:
    QScopedPointer<WWidgetManagerPrivate> d_ptr;

    /// Invokes @p cb through a direct or queued connection so it runs in @p receiver's thread.
    void invokeCallback(const SubscribeFunc &cb, QObject *receiver,
                        const WEvent &event);

    /// Drops all subscriptions held for a destroyed receiver.
    void onReceiverDestroyed(QObject *receiver);

    /**
     * @brief Adjusts a value before storing it as a widget attribute.
     *
     * For "Name", '#' is appended until the name is unique among widgets.
     */
    QVariant changeVariant(QObject *widget, const QString &key, const QVariant &value);

    /// Compiles a glob pattern (`*`, `**`, `?`) into a whole‑topic‑anchored QRegularExpression.
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