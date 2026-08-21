/**
 * @file wplugin.h
 * @brief Plugin base class providing loading, unloading, and metadata
 * management.
 *
 * WPlugin is a non‑QObject class that represents a single plugin. It manages
 * either a dynamically loaded library (DLL) or an external executable (EXE).
 *
 * @author howdy213
 * @date 2026-08-20
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
#ifndef WPLUGIN_H
#define WPLUGIN_H

#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QUuid>
#include <QVariant>

#include "WECore/Def/wedef.h"
#include "WECore/plugin/wplugininterface.h"
#include "WECore/plugin/wpluginstatemachine.h"

namespace we {

class WPluginManager; ///< Forward declaration of the plugin manager.
class WPluginPrivate;

/**
 * @brief Represents a single plugin with its metadata and loading state.
 *
 * WPlugin stores plugin configuration (metadata) and controls the
 * life‑cycle of the actual plugin component. The component can be
 * a Qt plugin (DLL/dylib) loaded via QPluginLoader, or an executable
 * launched as a virtual plugin.
 */
class WE_EXPORT WPlugin {
    Q_DISABLE_COPY(WPlugin)

public:
    /**
   * @brief Constructs a plugin instance.
   * @param parent The owning WPluginManager (must not be null after
   * construction).
   */
    explicit WPlugin(WPluginManager *parent);

    /// Destroys the plugin and releases all resources.
    virtual ~WPlugin();

    WPluginManager *parent() const;

    /**
   * @brief Reads plugin configuration from a file.
   * @param filePath The root path used to resolve relative plugin paths.
   * @param config   The full path to the configuration file.
   * @return @c true if the configuration was loaded successfully.
   */
    bool readConfig(const QString &filePath, QJsonObject config);

    /**
   * @brief Loads the actual plugin component (library or executable).
   * @return @c true if loading succeeded.
   */
    bool load();

    /**
   * @brief Unloads the plugin component and frees related resources.
   * @return @c true if the operation finished without errors.
   */
    bool unload();

    /**
   * @brief Indicates whether the plugin is currently loaded.
   * @return @c true if the plugin is in a loaded state.
   */
    bool available() const;

    /**
   * @brief Retrieves a metadata value.
   * @param key The metadata key.
   * @return The value associated with @p key, or a default-constructed
   * QVariant.
   */
    QVariant getMetaData(const QString &key) const;

    /**
   * @brief Checks whether a metadata key exists.
   * @param key The metadata key.
   * @return @c true if the key exists.
   */
    bool hasMetaData(const QString &key) const;

    /**
   * @brief Sets a metadata value.
   * @param key   The metadata key.
   * @param value The value to store.
   *
   * If the plugin is registered with a plugin manager, the manager's
   * persistent storage is also updated.
   */
    void setMetaData(const QString &key, const QVariant &value);

    /**
   * @brief Returns a pointer to the plugin interface.
   * @return A WPluginInterface pointer, or @c nullptr if the plugin is not
   *         loaded or does not provide a valid interface.
   */
    WPluginInterface *inst();

    /**
   * @brief Returns the plugin's metadata document.
   * @return A WMetaDocument object.
   */
    const WMetaDocument &getMetaDocument() const;

    /**
   * @brief Gets the current state of the plugin.
   * @return The current plugin state.
   */
    PluginState getState() const;

    /**
   * @brief Attempts to transition to a new state.
   * @param newState The target state.
   * @return true if transition was successful, false otherwise.
   */
    bool setState(PluginState newState);

    // the plugin version string.
    QString version() const;
    void setVersion(const QString &version);

    // the plugin display name.
    QString name() const;
    void setName(const QString &name);

    // the initialization arguments
    QString initArg() const;
    void setInitArg(const QString &initArg);

    // the absolute path to the plugin binary.
    QString path() const;
    void setPath(const QString &path);

    // the relative path (as stored in configuration).
    QString relativePath() const;
    void setRelativePath(const QString &relativePath);

    // the plugin build/creation date.
    QString date() const;
    void setDate(const QString &date);

    // the plugin author.
    QString author() const;
    void setAuthor(const QString &author);

    // the plugin description.
    QString desc() const;
    void setDesc(const QString &desc);

    // the plugin type (e.g. "dll" or "exe").
    QString type() const;
    void setType(const QString &type);

    // whether the plugin provides a main widget.
    bool mainWidget() const;
    void setMainWidget(bool mainWidget);

    // whether the plugin should be auto‑started. */
    bool autorun() const;
    void setAutorun(bool autorun);

    // whether the plugin requires administrator privileges.
    bool admin() const;
    void setAdmin(bool admin);

    // list of plugin dependencies (by UUID or name).
    QStringList depends() const;
    void setDepends(const QStringList &depends);

    // the list of dependency search paths.
    QStringList dependsPath() const;
    void setDependsPath(const QStringList &dependsPath);

    // the local UUID (assigned by the manager).
    QUuid localUuid() const;
    void setLocalUuid(const QUuid &uuid);

    // plugin's own UUID.
    QUuid uuid() const;
    void setUuid(const QUuid &uuid);

    // path to the configuration file.
    QString configPath() const;
    void setConfigPath(const QString &configPath);

private:
    bool loadDll(const QString &dllPath);
    bool loadExe(const QString &exePath);

    QScopedPointer<WPluginPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WPlugin)
};

} // namespace we

Q_DECLARE_METATYPE(we::WPlugin *)

#endif // WPLUGIN_H