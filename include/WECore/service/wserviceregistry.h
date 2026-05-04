/**
 * @file wserviceregistry.h
 * @brief Service registry for name‑to‑topic resolution and provider tracking.
 *
 * WServiceRegistry maintains a map of service names to request topics and
 * provider objects. It emits signals when services are registered or
 * unregistered and automatically removes entries when the provider is
 * destroyed.
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
#ifndef WSERVICEREGISTRY_H
#define WSERVICEREGISTRY_H

#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QStringList>

#include "WECore/def/wedef.h"

namespace we {

class WServiceRegistryPrivate;  ///< Forward declaration of private data.

/**
 * @brief Maintains a registry of services and their request topics.
 *
 * Services are identified by a unique name. Each service has an associated
 * request topic (used by the event bus) and a provider QObject. When the
 * provider is destroyed, the service is automatically unregistered.
 */
class WE_EXPORT WServiceRegistry : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(WServiceRegistry)

public:
    /**
     * @brief Constructs a service registry.
     * @param parent  Optional parent QObject.
     */
    explicit WServiceRegistry(QObject *parent = nullptr);

    /// Destroys the registry. Private data is automatically freed.
    ~WServiceRegistry() override;

    /**
     * @brief Registers a service.
     * @param serviceName   Unique service name.
     * @param requestTopic  Event bus topic used to invoke the service.
     * @param provider      QObject that provides the service.
     *
     * If a service with the same name already exists, it is replaced.
     * The registry will automatically unregister the service if the
     * provider is destroyed.
     */
    void registerService(const QString &serviceName,
                         const QString &requestTopic,
                         QObject *provider);

    /**
     * @brief Unregisters a service by name.
     * @param serviceName  Name of the service to remove.
     */
    void unregisterService(const QString &serviceName);

    /**
     * @brief Checks whether a service is registered.
     * @param serviceName  The service name.
     * @return @c true if the service exists.
     */
    bool hasService(const QString &serviceName) const;

    /**
     * @brief Returns the request topic associated with a service.
     * @param serviceName  The service name.
     * @return The topic string, or an empty string if the service is not found.
     */
    QString requestTopic(const QString &serviceName) const;

    /**
     * @brief Lists all currently registered service names.
     * @return A list of service names.
     */
    QStringList listServices() const;

signals:
    /// Emitted after a service has been successfully registered.
    void serviceRegistered(const QString &serviceName);

    /// Emitted when a service is unregistered (explicitly or due to provider destruction).
    void serviceUnregistered(const QString &serviceName);

private:
    QScopedPointer<WServiceRegistryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WServiceRegistry)
};

} // namespace we

Q_DECLARE_METATYPE(we::WServiceRegistry *)

#endif // WSERVICEREGISTRY_H