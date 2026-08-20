/**
 * @file wplugin.cpp
 * @brief Implementation of WPlugin.
 *
 * Contains the private data class WPluginPrivate and all method
 * implementations.
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

#include <QDebug>
#include <QJsonObject>
#include <QLibrary>
#include <QMetaObject>
#include <QPluginLoader>
#include <QRandomGenerator>

#include "WECore/file/wpath.h"
#include "WECore/metadata/wmetadocument.h"
#include "WECore/plugin/wplugininterface.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/plugin/wvirtualplugin.h"

#include "WECore/plugin/wplugin.h"

using namespace we::Consts;

namespace we {

/**
 * @brief Private implementation of WPlugin (d‑pointer pattern).
 *
 * Stores all internal state so that the public header can remain
 * stable across releases.
 */
class WPluginPrivate {
public:
    WMetaDocument doc;                ///< Metadata store.
    WPluginManager *parent = nullptr; ///< Owning plugin manager.
    QPluginLoader *loader = nullptr;  ///< Qt plugin loader (DLL mode).
    WPluginInterface *wif = nullptr;  ///< Plugin interface (DLL or EXE).
    bool loaded = false;              ///< Whether the plugin component is loaded.

    /// Describes the type of plugin backend.
    enum LoadFileType {
        TYPE_DLL,   ///< Shared library loaded via QPluginLoader.
        TYPE_EXE,   ///< External process treated as virtual plugin.
        TYPE_OTHER, ///< Reserved for future backends.
        TYPE_NONE   ///< No backend selected.
    } type = TYPE_NONE;
};

// Construction / Destruction

/// Constructs a WPlugin and initialises default metadata.
WPlugin::WPlugin(WPluginManager *parent) {
    d_ptr.reset(new WPluginPrivate);
    Q_D(WPlugin);
    d->parent = parent;
    d->doc.setDefaultValue("default");
    d->doc.setDefault(Plugin::Author, "");
    d->doc.setDefault(Plugin::Uuid, QUuid::createUuid().toString());
    d->doc.setDefault(Plugin::Date, __DATE__);
    d->doc.setDefault(Plugin::Desc, "");
    d->doc.setDefault(Plugin::Version, WE_VERSION);
    d->doc.setDefault(Plugin::Name,
                      QStringLiteral("Plugin-") +
                          QString::number(static_cast<qint64>(
                          QRandomGenerator::system()->generate())));
    d->doc.setDefault(Plugin::Path, "");
    
    // Initialize state machine
    m_stateMachine = new WPluginStateMachine(this);
}

/// Destructor. QScopedPointer automatically destroys WPluginPrivate.
WPlugin::~WPlugin() = default;

/// Returns the owning plugin manager.
WPluginManager *WPlugin::parent() const {
    const Q_D(WPlugin);
    return d->parent;
}

// Configuration

/// Loads configuration from a file and resolves relative paths.
bool WPlugin::readConfig(const QString &filePath, QJsonObject config) {
    Q_D(WPlugin);
    if (!d->doc.load(QJsonDocument(config).toJson(), false))
        return false;

    const QString relativePath = qvariant_cast<QString>(d->doc.get(Plugin::RelativePath));
    const QString absolutePath = WPath().resolvePath(filePath, relativePath);
    d->doc.set(Plugin::Path, absolutePath);
    d->doc.set(Plugin::ConfigPath, filePath);
    return true;
}

// Loading / Unloading

/// Loads the plugin backend (DLL or EXE). Returns false if already loaded.
bool WPlugin::load() {
    Q_D(WPlugin);
    
    // Debug: Print current state before transition
    qDebug() << "WPlugin::load: Current state before transition:" << static_cast<int>(getState());
    
    // Use state machine for loading
    if (!setState(PluginState::Loading)) {
        qWarning("WPlugin::load: Cannot transition to loading state");
        return false;
    }

    if (!d->parent) {
        setState(PluginState::Error);
        return false;
    }

    const QString filepath = qvariant_cast<QString>(getMetaData(Plugin::Path));
    bool loadResult = false;
    
    if (QLibrary::isLibrary(filepath)) {
        loadResult = loadDll(filepath);
    } else if (filepath.endsWith(QLatin1String(".exe")) ||
               filepath.endsWith(QLatin1String(".bat"))) {
        loadResult = loadExe(filepath);
    } else {
        qWarning("WPlugin::load: Unsupported plugin file type");
        setState(PluginState::Error);
        return false;
    }
    
    // Debug: Print load result and final state
    qDebug() << "WPlugin::load: Load result:" << loadResult << "Final state:" << static_cast<int>(getState());
    
    if (loadResult) {
        setState(PluginState::Loaded);
        return true;
    } else {
        setState(PluginState::Error);
        return false;
    }
}

/// Unloads the plugin backend and frees all related resources.
bool WPlugin::unload() {
    Q_D(WPlugin);
    
    // Use state machine for unloading
    if (!setState(PluginState::Unloading)) {
        qWarning("WPlugin::unload: Cannot transition to unloading state");
        return false;
    }

    bool ret = true;

    // Let the manager handle its own bookkeeping first.
    if (d->parent) {
        //ret = d->parent->unloadPlugin(this);
        //d->parent = nullptr;
    }

    WPluginInterface *iface =
        qobject_cast<WPluginInterface *>(d->loader->instance());
    WMessage msg;
    iface->deinit(msg);

    // Release DLL resources.
    if (d->type == WPluginPrivate::TYPE_DLL && d->loader) {
        if (!d->loader->unload())
            ret = false;
        delete d->loader;
        d->loader = nullptr;
    }
    // Release EXE virtual plugin.
    else if (d->type == WPluginPrivate::TYPE_EXE && d->wif) {
        delete d->wif;
        d->wif = nullptr;
    }

    d->loaded = false;
    d->type = WPluginPrivate::TYPE_NONE;
    
    // Transition to unloaded state
    setState(PluginState::Unloaded);
    return ret;
}

/// Returns whether the plugin component is currently loaded.
bool WPlugin::available() const {
    Q_D(const WPlugin);
    return d->loaded;
}

// Metadata access

/// Retrieves a metadata value.
QVariant WPlugin::getMetaData(const QString &key) const {
    Q_D(const WPlugin);
    return d->doc.get(key);
}

/// Checks whether a given metadata key exists.
bool WPlugin::hasMetaData(const QString &key) const {
    Q_D(const WPlugin);
    return d->doc.hasArg(key);
}

/// Returns the unique ID assigned by the manager.
QUuid WPlugin::getLocalUuid() const {
    QUuid id = QUuid(getMetaData(Plugin::LocalUuid).toString());
    return id;
}

/// Returns the local UUID assigned to the plugin.
QUuid WPlugin::getUuid() const {
    QUuid id = getMetaData(Plugin::Uuid).toUuid();
    return id;
}

/// Stores a metadata value, syncing it with the manager if present.
void WPlugin::setMetaData(const QString &key, const QVariant &value) {
    Q_D(WPlugin);
    QVariant stored = value;
    if (d->parent)
        stored = d->parent->setPluginData(this->getLocalUuid(), key, value);
    d->doc.set(key, stored);
}

// Backend helpers

/// Loads a Qt plugin (DLL) using QPluginLoader.
bool WPlugin::loadDll(const QString &dllPath) {
    Q_D(WPlugin);
    if (!QLibrary::isLibrary(dllPath))
        return false;

    auto *loader = new QPluginLoader(dllPath);
    if (loader->load()) {
        WPluginInterface *iface =
            qobject_cast<WPluginInterface *>(loader->instance());
        if (iface) {
            d->loader = loader;
            d->loaded = true;
            setMetaData(Plugin::Type, QStringLiteral("dll"));
            QJsonObject meta = loader->metaData().value("MetaData").toObject();
            if (meta.empty())
                qDebug() << "No metadata for " << dllPath;
            for (auto it = meta.begin(); it != meta.end(); ++it) {
                setMetaData(it.key(), it.value().toVariant());
            }
        } else {
            qWarning("WPlugin::loadDll: Plugin does not implement WPluginInterface");
            delete loader;
            loader = nullptr;
        }
    } else {
        qWarning() << "WPlugin::loadDll: Failed to load" << dllPath << "-"
                   << loader->errorString();
        delete loader;
        loader = nullptr;
    }

    d->type = WPluginPrivate::TYPE_DLL;
    return d->loaded;
}

/// Loads an executable as a virtual plugin.
bool WPlugin::loadExe(const QString &exePath) {
    Q_D(WPlugin);
    d->loader = nullptr;
    d->loaded = true;
    d->type = WPluginPrivate::TYPE_EXE;

    auto *vp = new WVirtualPlugin;
    vp->setPlugin(this);
    vp->setFile(exePath);
    setMetaData(Plugin::Type, QStringLiteral("exe"));

    // Derive a display name from the file name.
    const QStringList parts = exePath.split(QChar('/'));
    if (!parts.isEmpty()) {
        const QString fileName = parts.last().split(QChar('.')).first();
        if (!fileName.isEmpty())
            setMetaData(Plugin::Name, fileName);
    }
    d->wif = vp;
    return true;
}

// Interface access

/// Returns the plugin interface pointer (DLL or EXE virtual plugin).
WPluginInterface *WPlugin::inst() {
    Q_D(WPlugin);
    switch (d->type) {
    case WPluginPrivate::TYPE_DLL:
        return d->loader ? qobject_cast<WPluginInterface *>(d->loader->instance())
                         : nullptr;
    case WPluginPrivate::TYPE_EXE:
        return d->wif;
    case WPluginPrivate::TYPE_OTHER:
    case WPluginPrivate::TYPE_NONE:
        break;
    }
    return nullptr;
}

/// Returns the plugin's metadata document.
const WMetaDocument& WPlugin::getMetaDocument() const {
    Q_D(const WPlugin);
    return d->doc;
}

/// Gets the current state of the plugin.
PluginState WPlugin::getState() const {
    return m_stateMachine ? m_stateMachine->currentState() : PluginState::Unloaded;
}

/// Attempts to transition to a new state.
bool WPlugin::setState(PluginState newState) {
    return m_stateMachine && m_stateMachine->transitionTo(newState);
}
} // namespace we