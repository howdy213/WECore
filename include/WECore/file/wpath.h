/**
 * @file wpath.h
 * @brief Path utility class for resolving module paths and folders.
 *
 * WPath provides methods to obtain the executable’s file path, to
 * retrieve the file system location of a loaded plugin, and to
 * resolve relative paths against a base directory.
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
 * WPath works with the WEBase and plugin manager to resolve file paths
 * and directories for the main executable and loaded plugins. Relative
 * paths can be turned into absolute paths using resolvePath().
 */
class WE_EXPORT WPath
{
    Q_DISABLE_COPY(WPath)

public:
    /**
     * @brief Constructs a WPath instance.
     * @param we  Optional pointer to WEBase; can be set later with setWEBase().
     */
    explicit WPath(WEBase *we = nullptr);

    /// Destroys the instance. Private data is automatically cleaned up.
    virtual ~WPath();

    /**
     * @brief Sets or replaces the WEBase reference.
     * @param base  Pointer to the application base.
     */
    void setWEBase(WEBase *base);

    /**
     * @brief Returns the full path of the current executable.
     */
    QString getModulePath() const;

    /**
     * @brief Returns the directory containing the current executable.
     */
    QString getModuleFolder() const;

    /**
     * @brief Retrieves the file path of a plugin identified by its UUID.
     * @param moduleId  The plugin UUID.
     * @return The stored path, or an empty string if not found.
     */
    QString getModulePath(const QUuid &moduleId) const;

    /**
     * @brief Retrieves the directory containing the plugin’s file.
     * @param moduleId  The plugin UUID.
     * @return The directory, or an empty string.
     */
    QString getModuleFolder(const QUuid &moduleId) const;

    /**
     * @brief Retrieves the file path of a plugin from its WPlugin instance.
     * @param plugin  Pointer to the plugin.
     * @return The stored path, or an empty string.
     */
    QString getModulePath(WPlugin *plugin) const;

    /**
     * @brief Retrieves the directory containing the plugin’s file.
     * @param plugin  Pointer to the plugin.
     * @return The directory, or an empty string.
     */
    QString getModuleFolder(WPlugin *plugin) const;

    /**
     * @brief Extracts the directory part of a file path (ends with '/').
     * @param path  A file path.
     * @return The directory portion, e.g., "C:/folder/" from "C:/folder/file.exe".
     */
    QString splitPath(const QString &path) const;

    /**
     * @brief Resolves a possibly‑relative path @p des against @p cur.
     * @param cur  Reference path (file or directory).
     * @param des  Target path (absolute or relative).
     * @return The absolute cleaned path.
     *
     * If @p cur is a file, its directory is used as the base.
     */
    QString resolvePath(const QString &cur, const QString &des) const;

private:
    QScopedPointer<WPathPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WPath)
};

} // namespace we

Q_DECLARE_METATYPE(we::WPath)
Q_DECLARE_METATYPE(we::WPath *)

#endif // WPATH_H