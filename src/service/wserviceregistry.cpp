/**
 * @file wserviceregistry.cpp
 * @brief Implementation of WServiceRegistry.
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
#include "WECore/service/wserviceregistry.h"

#include <QMap>
#include <QPointer>

namespace we {

/**
 * @brief Private data for WServiceRegistry (d-pointer pattern).
 */
class WServiceRegistryPrivate
{
public:
    /// Describes a single registered service.
    struct ServiceInfo {
        QString requestTopic;        ///< Event bus topic for invoking the service.
        QPointer<QObject> provider;  ///< Weak pointer: cleared when the provider dies.
    };

    QMap<QString, ServiceInfo> services; ///< Service name -> information.
};

WServiceRegistry::WServiceRegistry(QObject *parent)
    : QObject(parent)
    , d_ptr(new WServiceRegistryPrivate)
{
}

WServiceRegistry::~WServiceRegistry() = default;

void WServiceRegistry::registerService(const QString &serviceName,
                                       const QString &requestTopic,
                                       QObject *provider)
{
    Q_D(WServiceRegistry);
    if (serviceName.isEmpty() || requestTopic.isEmpty() || !provider)
        return;

    // Replacing an existing name keeps the registry single-valued per service.
    if (d->services.contains(serviceName)) {
        unregisterService(serviceName);
    }

    WServiceRegistryPrivate::ServiceInfo info{requestTopic, provider};
    d->services.insert(serviceName, info);

    // Auto-unregister so a destroyed provider never leaves a stale entry.
    connect(provider, &QObject::destroyed, this, [this, serviceName]() {
        unregisterService(serviceName);
    });

    emit serviceRegistered(serviceName);
}

void WServiceRegistry::unregisterService(const QString &serviceName)
{
    Q_D(WServiceRegistry);
    if (d->services.remove(serviceName)) {
        emit serviceUnregistered(serviceName);
    }
}

bool WServiceRegistry::hasService(const QString &serviceName) const
{
    Q_D(const WServiceRegistry);
    return d->services.contains(serviceName);
}

QString WServiceRegistry::requestTopic(const QString &serviceName) const
{
    Q_D(const WServiceRegistry);
    auto it = d->services.constFind(serviceName);
    return (it != d->services.constEnd()) ? it->requestTopic : QString();
}

QStringList WServiceRegistry::listServices() const
{
    Q_D(const WServiceRegistry);
    return d->services.keys();
}

} // namespace we