/**
 * @file wpath.h
 * @brief Path utility class for resolving module paths and folders.
 *
 * WPath provides methods to obtain the executable’s file path, to
 * retrieve the file system location of a loaded plugin, and to
 * resolve relative paths against a base directory.
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
#ifndef WPATH_H
#define WPATH_H

#include <QObject>
#include <QString>
#include <QUuid>
#include <QtGlobal>

#include <Windows.h>

#include "WECore/def/wedef.h"

namespace we {

class WPathPrivate;

/**
 * @brief Utility for path operations related to modules and the application.
 *
 * Resolves file paths and directories for the main executable and loaded
 * plugins, and turns relative paths into absolute ones with resolvePath().
 * The executable path is queried through the Win32 API, so WPath is
 * Windows‑only.
 */
class WE_EXPORT WPath
{
    Q_DISABLE_COPY(WPath)

public:
    /// Constructs a WPath; @p we may also be supplied later via setWEBase().
    explicit WPath(WEBase *we = nullptr);

    virtual ~WPath();

    /// Sets or replaces the WEBase used to look up plugin paths.
    void setWEBase(WEBase *base);

    /// Full path of the current executable (Win32 GetModuleFileName).
    QString getModulePath() const;

    /// Directory containing the current executable, including the trailing '/'.
    QString getModuleFolder() const;

    /// Path of the plugin identified by @p moduleId, or an empty string if unknown.
    QString getModulePath(const QUuid &moduleId) const;

    /// Directory containing the plugin's file, or an empty string.
    QString getModuleFolder(const QUuid &moduleId) const;

    /// Path of @p plugin, or an empty string if @p plugin is null.
    QString getModulePath(WPlugin *plugin) const;

    /// Directory containing the plugin's file, or an empty string.
    QString getModuleFolder(WPlugin *plugin) const;

    /// Directory part of @p path, including the trailing '/'; empty if it has no '/'.
    QString splitPath(const QString &path) const;

    /// Resolves @p des against @p cur; if @p cur is a file, its directory is used as the base.
    QString resolvePath(const QString &cur, const QString &des) const;

private:
    QScopedPointer<WPathPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WPath)
};

} // namespace we

Q_DECLARE_METATYPE(we::WPath)
Q_DECLARE_METATYPE(we::WPath *)

#endif // WPATH_H