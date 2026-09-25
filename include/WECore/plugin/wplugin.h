/**
 * @file wplugin.h
 * @brief Plugin base class providing loading, unloading, and metadata
 * management.
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
#ifndef WPLUGIN_H
#define WPLUGIN_H

#include <QCoreApplication>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QUuid>
#include <QVariant>

#include "WECore/def/wedef.h"
#include "WECore/plugin/wplugininterface.h"
#include "WECore/plugin/wpluginstatemachine.h"

namespace we {

class WPluginManager;
class WPluginPrivate;

/**
 * @brief Represents a single plugin with its metadata and loading state.
 *
 * Not a QObject. The backend is either a Qt plugin (DLL/dylib) loaded through
 * QPluginLoader or a WVirtualPlugin that wraps an external executable (EXE).
 * Metadata lives in a WMetaDocument and is kept in sync with the owning
 * WPluginManager, which also enforces unique display names.
 */
class WE_EXPORT WPlugin {
    Q_DISABLE_COPY(WPlugin)

public:
    /**
     * @brief Constructs a plugin.
     * @param parent Owning manager. May be null, but load() then fails.
     */
    explicit WPlugin(WPluginManager *parent);

    virtual ~WPlugin();

    /// Owning manager, or null if none was supplied.
    WPluginManager *parent() const;

    /**
     * @brief Loads metadata from an already-parsed configuration object.
     * @param filePath Root directory used to resolve the relative plugin path.
     * @param config   Parsed configuration object.
     *
     * Stores the resolved absolute path in Plugin::Path and remembers
     * @p filePath as Plugin::ConfigPath.
     */
    bool readConfig(const QString &filePath, QJsonObject config);

    /**
     * @brief Loads the backend (DLL or EXE) through the state machine.
     * @return false if the Loading transition is rejected, the plugin has no
     *         manager, or the file type is unsupported.
     */
    bool load();

    /// Unloads the backend and releases the library / interface resources.
    bool unload();

    /// Whether the backend is currently loaded.
    bool available() const;

    /// Metadata value for @p key; a default-constructed QVariant if absent.
    QVariant getMetaData(const QString &key) const;

    bool hasMetaData(const QString &key) const;

    /// Stores metadata; routed through the manager when one is set.
    void setMetaData(const QString &key, const QVariant &value);

    /// Plugin interface, or null when not loaded or not implementing it.
    WPluginInterface *inst();

    const WMetaDocument &getMetaDocument() const;

    PluginState getState() const;

    /// Requests a state transition; false if the transition is invalid.
    bool setState(PluginState newState);

    QString version() const;
    void setVersion(const QString &version);

    QString name() const;
    void setName(const QString &name);

    /// Initialization arguments passed to the plugin.
    QString initArg() const;
    void setInitArg(const QString &initArg);

    /// Absolute path to the plugin binary.
    QString path() const;
    void setPath(const QString &path);

    /// Path as stored in the configuration.
    QString relativePath() const;
    void setRelativePath(const QString &relativePath);

    QString date() const;
    void setDate(const QString &date);

    QString author() const;
    void setAuthor(const QString &author);

    QString desc() const;
    void setDesc(const QString &desc);

    /// Plugin backend type, e.g. "dll" or "exe".
    QString type() const;
    void setType(const QString &type);

    bool mainWidget() const;
    void setMainWidget(bool mainWidget);

    bool autorun() const;
    void setAutorun(bool autorun);

    bool admin() const;
    void setAdmin(bool admin);

    /// Plugin dependencies, identified by UUID or name.
    QStringList depends() const;
    void setDepends(const QStringList &depends);

    /// Search paths used to resolve dependencies.
    QStringList dependsPath() const;
    void setDependsPath(const QStringList &dependsPath);

    /// Instance UUID assigned by the manager (not the plugin's own UUID).
    QUuid localUuid() const;
    void setLocalUuid(const QUuid &uuid);

    /// UUID declared by the plugin itself.
    QUuid uuid() const;
    void setUuid(const QUuid &uuid);

    QString configPath() const;
    void setConfigPath(const QString &configPath);

    Q_DECLARE_TR_FUNCTIONS(WPlugin)

private:
    bool loadDll(const QString &dllPath);
    bool loadExe(const QString &exePath);

    QScopedPointer<WPluginPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WPlugin)
};

} // namespace we

Q_DECLARE_METATYPE(we::WPlugin *)

#endif // WPLUGIN_H