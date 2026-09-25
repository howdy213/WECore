/**
 * @file wserviceproxy.h
 * @brief Asynchronous service call proxy.
 *
 * WServiceProxy provides a convenient interface for calling services
 * registered in a WServiceRegistry via an event bus (WWidgetManager).
 * It supports both future-based and callback-based invocation patterns.
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
 * The proxy resolves a service name to a request topic via WServiceRegistry
 * and delegates the actual call to WWidgetManager::request(). Results are
 * delivered as a QFuture<QVariant> or through a callback.
 *
 * The bus and registry are borrowed: they must outlive the proxy, which
 * does not take ownership of either. The proxy itself is not a QObject.
 */
class WE_EXPORT WServiceProxy
{
    Q_DISABLE_COPY(WServiceProxy)

public:
    WServiceProxy(WWidgetManager *bus, WServiceRegistry *registry);

    ~WServiceProxy();

    /**
     * @brief Calls a service asynchronously.
     *
     * The returned future is fulfilled with the reply, or holds an exception
     * if the service is unknown, the bus fails, or @p timeoutMs elapses.
     */
    QFuture<QVariant> call(const QString &serviceName,
                           const QVariant &data = {},
                           int timeoutMs = 5000);

    /**
     * @brief Calls a service and delivers the outcome to @p callback.
     *
     * @p callback has the signature `void(bool ok, QVariant result)`. It is
     * always invoked (also on failure), with @p ok reporting success; on
     * failure @p result carries the error text.
     */
    template <typename Func>
    void call(const QString &serviceName, const QVariant &data,
              Func callback, int timeoutMs = 5000);

private:
    QScopedPointer<WServiceProxyPrivate> d_ptr;
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