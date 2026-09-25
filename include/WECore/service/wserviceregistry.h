/**
 * @file wserviceregistry.h
 * @brief Service registry for name-to-topic resolution and provider tracking.
 *
 * WServiceRegistry maintains a map of service names to request topics and
 * provider objects. It emits signals when services are registered or
 * unregistered and automatically removes entries when the provider is
 * destroyed.
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
#ifndef WSERVICEREGISTRY_H
#define WSERVICEREGISTRY_H

#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QStringList>

#include "WECore/def/wedef.h"

namespace we {

class WServiceRegistryPrivate;

/**
 * @brief Maintains a registry of services and their request topics.
 *
 * Services are identified by a unique name. Each service has an associated
 * request topic (used by the event bus) and a provider QObject. When the
 * provider is destroyed, the service is automatically unregistered.
 *
 * Not thread-safe: use the registry from the thread that owns it.
 */
class WE_EXPORT WServiceRegistry : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(WServiceRegistry)

public:
    explicit WServiceRegistry(QObject *parent = nullptr);
    ~WServiceRegistry() override;

    /**
     * @brief Registers a service.
     *
     * A service with the same name is replaced. The registry watches the
     * provider and unregisters the service when the provider is destroyed.
     * Registration is ignored if the name or topic is empty, or if the
     * provider is null.
     */
    void registerService(const QString &serviceName,
                         const QString &requestTopic,
                         QObject *provider);

    /// Removes a service. No-op if @p serviceName is not registered.
    void unregisterService(const QString &serviceName);

    /// Returns @c true if a service with @p serviceName is registered.
    bool hasService(const QString &serviceName) const;

    /// Returns the request topic of @p serviceName, or an empty string if unknown.
    QString requestTopic(const QString &serviceName) const;

    /// Returns the names of all currently registered services.
    QStringList listServices() const;

signals:
    /// Emitted after a service has been successfully registered.
    void serviceRegistered(const QString &serviceName);

    /// Emitted when a service is unregistered, explicitly or via provider destruction.
    void serviceUnregistered(const QString &serviceName);

private:
    QScopedPointer<WServiceRegistryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WServiceRegistry)
};

} // namespace we

Q_DECLARE_METATYPE(we::WServiceRegistry *)

#endif // WSERVICEREGISTRY_H