/**
 * @file wserviceproxy.h
 * @brief Asynchronous service call proxy.
 *
 * WServiceProxy provides a convenient interface for calling services
 * registered in a WServiceRegistry via an event bus (WWidgetManager).
 * It supports both future‑based and callback‑based invocation patterns.
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
#ifndef WSERVICEPROXY_H
#define WSERVICEPROXY_H

#include <QFuture>
#include <QFutureWatcher>
#include <QPointer>
#include <QPromise>
#include <QScopedPointer>
#include <QString>
#include <QVariant>

#include "WECore/def/wedef.h"
#include "WECore/widget/wwidgetmanager.h"
#include "WECore/service/wserviceregistry.h"

namespace we {

class WServiceProxyPrivate;

/**
 * @brief An asynchronous proxy for invoking services through an event bus.
 *
 * The proxy uses WServiceRegistry to resolve a service name to a request
 * topic, then delegates the actual call to WWidgetManager::request().
 * Results are returned either as a QFuture<QVariant> or via a callback.
 */
class WE_EXPORT WServiceProxy
{
    Q_DISABLE_COPY(WServiceProxy)

public:
    /**
     * @brief Constructs a service proxy.
     * @param bus       The event bus used for communication.
     * @param registry  The service registry for name → topic resolution.
     */
    WServiceProxy(WWidgetManager *bus, WServiceRegistry *registry);

    /// Destroys the proxy. Private data is automatically freed.
    ~WServiceProxy();

    /**
     * @brief Asynchronously calls a service, returning a future.
     * @param serviceName  Name of the service to invoke.
     * @param data         Payload to send (default empty).
     * @param timeoutMs    Timeout in milliseconds (default 5000).
     * @return A QFuture that will eventually contain the result or an exception.
     */
    QFuture<QVariant> call(const QString &serviceName,
                           const QVariant &data = {},
                           int timeoutMs = 5000);

    /**
     * @brief Calls a service with a callback.
     * @param serviceName  Name of the service.
     * @param data         Payload.
     * @param callback     Function with signature `void(bool ok, QVariant result)`.
     * @param timeoutMs    Timeout in milliseconds (default 5000).
     *
     * The callback is always invoked (even on error), with @p ok
     * indicating success.
     */
    template <typename Func>
    void call(const QString &serviceName, const QVariant &data,
              Func callback, int timeoutMs = 5000);

private:
    QScopedPointer<WServiceProxyPrivate> d_ptr;  ///< Private data object.
};

// --------------------------------------------------------------------------
// Template implementation
// --------------------------------------------------------------------------

template <typename Func>
void WServiceProxy::call(const QString &serviceName, const QVariant &data,
                         Func callback, int timeoutMs)
{
    QFuture<QVariant> future = call(serviceName, data, timeoutMs);
    auto *watcher = new QFutureWatcher<QVariant>();
    QObject::connect(watcher, &QFutureWatcher<QVariant>::finished,
                     [watcher, callback]() {
                         watcher->deleteLater();
                         try {
                             QVariant result = watcher->result();
                             callback(true, result);
                         } catch (const std::exception &e) {
                             callback(false,
                                      QVariant::fromValue(QString::fromStdString(e.what())));
                         }
                     });
    watcher->setFuture(future);
}

} // namespace we

#endif // WSERVICEPROXY_H