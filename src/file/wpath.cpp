/**
 * @file wpath.cpp
 * @brief Implementation of WPath.
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
#include "WECore/file/wpath.h"

#include <QDir>
#include <QFileInfo>

#include "WECore/plugin/wplugin.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/we/webase.h"

using namespace we::Consts;

namespace we {

/**
 * @brief Private data for WPath.
 *
 * Stores a weak reference to WEBase and the current module name.
 */
class WPathPrivate
{
public:
    QString moduleName;   ///< Name of the current module (unused in this version).
    WEBase *we = nullptr; ///< Pointer to the application base.
};

WPath::WPath(WEBase *we)
    : d_ptr(new WPathPrivate)
{
    Q_D(WPath);
    d->we = we;
}

WPath::~WPath() = default;

// WEBase setter
void WPath::setWEBase(WEBase *base)
{
    Q_D(WPath);
    d->we = base;
}

// Path queries for the main executable
QString WPath::getModulePath() const
{
    WCHAR buffer[MAX_PATH];
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    QString path = QString::fromWCharArray(buffer);
    return QDir::cleanPath(path);
}

QString WPath::getModuleFolder() const
{
    return splitPath(getModulePath());
}

// Path queries for plugins (by UUID or WPlugin pointer)
QString WPath::getModulePath(const QUuid &moduleId) const
{
    Q_D(const WPath);
    if (!d->we)
        return {};

    auto *pm = d->we->getWEClass()->pluginManager();
    auto *plugin = pm->getPluginById(moduleId);
    if (!plugin)
        return {};

    return qvariant_cast<QString>(plugin->getMetaData(Plugin::Path));
}

QString WPath::getModuleFolder(const QUuid &moduleId) const
{
    return splitPath(getModulePath(moduleId));
}

QString WPath::getModulePath(WPlugin *plugin) const
{
    Q_D(const WPath);
    if (!d->we || !plugin)
        return {};

    return qvariant_cast<QString>(plugin->getMetaData(Plugin::Path));
}

QString WPath::getModuleFolder(WPlugin *plugin) const
{
    return splitPath(getModulePath(plugin));
}

// Path manipulation helpers
QString WPath::splitPath(const QString &path) const
{
    const QString clean = QDir::cleanPath(path);
    const int lastSlash = clean.lastIndexOf(QLatin1Char('/'));
    return (lastSlash >= 0) ? clean.left(lastSlash + 1) : QString();
}

QString WPath::resolvePath(const QString &cur, const QString &des) const
{
    QString curClean = QDir::cleanPath(cur);
    QString desClean = QDir::cleanPath(des);
    QFileInfo info(curClean);

    if (info.isFile()) {
        curClean = splitPath(curClean);
    }

    QDir currentDir(curClean);

    if (QDir::isAbsolutePath(desClean))
        return QDir::cleanPath(desClean);

    return QDir::cleanPath(currentDir.absoluteFilePath(desClean));
}

} // namespace we