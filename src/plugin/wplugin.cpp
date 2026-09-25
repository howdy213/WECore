/**
 * @file wplugin.cpp
 * @brief Implementation of WPlugin.
 *
 * Contains the private data class WPluginPrivate and all method
 * implementations.
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
 * @brief Private state of WPlugin (d-pointer pattern).
 *
 * doc holds the metadata. The DLL backend is owned through loader; the EXE
 * backend is the wif virtual plugin. Both are released in unload().
 */
class WPluginPrivate {
public:
    WMetaDocument doc;                ///< Metadata store.
    WPluginManager *parent = nullptr; ///< Owning plugin manager.
    QPluginLoader *loader = nullptr;  ///< Qt plugin loader (DLL mode).
    WPluginInterface *wif = nullptr;  ///< Interface of the EXE (virtual) backend.
    bool loaded = false;              ///< Whether the backend is loaded.
    WPluginStateMachine *m_stateMachine = nullptr; ///< Owned lifecycle tracker.

    /// Which backend is in use.
    enum LoadFileType {
        TYPE_DLL,   ///< Shared library loaded via QPluginLoader.
        TYPE_EXE,   ///< External process wrapped by WVirtualPlugin.
        TYPE_OTHER, ///< Reserved for future backends.
        TYPE_NONE   ///< No backend selected.
    } type = TYPE_NONE;
};

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

    d->m_stateMachine = new WPluginStateMachine(this);
}

// d_ptr frees WPluginPrivate (and with it the state machine).
WPlugin::~WPlugin() = default;

WPluginManager *WPlugin::parent() const {
    const Q_D(WPlugin);
    return d->parent;
}

/// Resolves Plugin::RelativePath against @p filePath and stores the result.
bool WPlugin::readConfig(const QString &filePath, QJsonObject config) {
    Q_D(WPlugin);
    if (!d->doc.load(QJsonDocument(config).toJson(), false))
        return false;

    const QString relativePath =
        qvariant_cast<QString>(d->doc.get(Plugin::RelativePath));
    const QString absolutePath = WPath().resolvePath(filePath, relativePath);
    d->doc.set(Plugin::Path, absolutePath);
    d->doc.set(Plugin::ConfigPath, filePath);
    return true;
}

// The state machine owns the sequence; every failure leaves the plugin in Error.
bool WPlugin::load() {
    Q_D(WPlugin);

    qDebug() << "WPlugin::load: Current state before transition:"
             << static_cast<int>(getState());

    if (!setState(PluginState::Loading)) {
        qWarning() << tr("WPlugin::load: Cannot transition to loading state");
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
        qWarning() << tr("WPlugin::load: Unsupported plugin file type");
        setState(PluginState::Error);
        return false;
    }

    qDebug() << "WPlugin::load: Load result:" << loadResult
             << "Final state:" << static_cast<int>(getState());

    if (loadResult) {
        setState(PluginState::Loaded);
        return true;
    } else {
        setState(PluginState::Error);
        return false;
    }
}

/// Unloads the backend and frees all related resources.
bool WPlugin::unload() {
    Q_D(WPlugin);

    if (!setState(PluginState::Unloading)) {
        qWarning() << tr("WPlugin::unload: Cannot transition to unloading state");
        return false;
    }

    bool ret = true;

    WPluginInterface *iface = inst();
    if (iface) {
        WMessage msg;
        iface->deinit(msg);
    }

    // Release the backend: unload a shared library, delete a virtual plugin.
    if (d->type == WPluginPrivate::TYPE_DLL && d->loader) {
        if (!d->loader->unload())
            ret = false;
        delete d->loader;
        d->loader = nullptr;
    } else if (d->type == WPluginPrivate::TYPE_EXE && d->wif) {
        delete d->wif;
        d->wif = nullptr;
    }

    d->loaded = false;
    d->type = WPluginPrivate::TYPE_NONE;

    setState(PluginState::Unloaded);
    return ret;
}

bool WPlugin::available() const {
    Q_D(const WPlugin);
    return d->loaded;
}

QVariant WPlugin::getMetaData(const QString &key) const {
    Q_D(const WPlugin);
    return d->doc.get(key);
}

bool WPlugin::hasMetaData(const QString &key) const {
    Q_D(const WPlugin);
    return d->doc.hasArg(key);
}

// When registered, the manager may rewrite the value (e.g. to keep names unique).
void WPlugin::setMetaData(const QString &key, const QVariant &value) {
    Q_D(WPlugin);
    QVariant stored = value;
    if (d->parent)
        stored = d->parent->setPluginData(localUuid(), key, value);
    d->doc.set(key, stored);
}

/// Loads a Qt plugin (DLL) and merges its embedded metadata into ours.
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
            setType(QStringLiteral("dll"));
            QJsonObject meta = loader->metaData().value("MetaData").toObject();
            if (meta.empty())
                qDebug() << "No metadata for " << dllPath;
            for (auto it = meta.begin(); it != meta.end(); ++it) {
                setMetaData(it.key(), it.value().toVariant());
            }
        } else {
            qWarning() << tr("WPlugin::loadDll: Plugin does not implement "
                             "WPluginInterface");
            delete loader;
            loader = nullptr;
        }
    } else {
        qWarning() << tr("WPlugin::loadDll: Failed to load %1 - %2")
                          .arg(dllPath, loader->errorString());
        delete loader;
        loader = nullptr;
    }

    d->type = WPluginPrivate::TYPE_DLL;
    return d->loaded;
}

/// Loads an executable as a virtual plugin (no QPluginLoader involved).
bool WPlugin::loadExe(const QString &exePath) {
    Q_D(WPlugin);
    d->loader = nullptr;
    d->loaded = true;
    d->type = WPluginPrivate::TYPE_EXE;

    auto *vp = new WVirtualPlugin;
    vp->setPlugin(this);
    vp->setFile(exePath);
    setType(QStringLiteral("exe"));

    const QStringList parts = exePath.split(QChar('/'));
    if (!parts.isEmpty()) {
        const QString fileName = parts.last().split(QChar('.')).first();
        if (!fileName.isEmpty())
            setMetaData(Plugin::Name, fileName);
    }
    d->wif = vp;
    return true;
}

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

const WMetaDocument &WPlugin::getMetaDocument() const {
    Q_D(const WPlugin);
    return d->doc;
}

PluginState WPlugin::getState() const {
    const Q_D(WPlugin);
  return d->m_stateMachine ? d->m_stateMachine->currentState()
                             : PluginState::Unloaded;
}

bool WPlugin::setState(PluginState newState) {
    Q_D(WPlugin);
    return d->m_stateMachine && d->m_stateMachine->transitionTo(newState);
}

QString WPlugin::version() const {
    return getMetaData(Plugin::Version).toString();
}
void WPlugin::setVersion(const QString &version) {
    setMetaData(Plugin::Version, version);
}

QString WPlugin::name() const { return getMetaData(Plugin::Name).toString(); }
void WPlugin::setName(const QString &name) { setMetaData(Plugin::Name, name); }

QString WPlugin::initArg() const {
    return getMetaData(Plugin::Init).toString();
}
void WPlugin::setInitArg(const QString &initArg) {
    setMetaData(Plugin::Init, initArg);
}

QString WPlugin::path() const { return getMetaData(Plugin::Path).toString(); }
void WPlugin::setPath(const QString &path) { setMetaData(Plugin::Path, path); }

QString WPlugin::relativePath() const {
    return getMetaData(Plugin::RelativePath).toString();
}
void WPlugin::setRelativePath(const QString &relativePath) {
    setMetaData(Plugin::RelativePath, relativePath);
}

QString WPlugin::date() const { return getMetaData(Plugin::Date).toString(); }
void WPlugin::setDate(const QString &date) { setMetaData(Plugin::Date, date); }

QString WPlugin::author() const {
    return getMetaData(Plugin::Author).toString();
}
void WPlugin::setAuthor(const QString &author) {
    setMetaData(Plugin::Author, author);
}

QString WPlugin::desc() const { return getMetaData(Plugin::Desc).toString(); }
void WPlugin::setDesc(const QString &desc) { setMetaData(Plugin::Desc, desc); }

QString WPlugin::type() const { return getMetaData(Plugin::Type).toString(); }
void WPlugin::setType(const QString &type) { setMetaData(Plugin::Type, type); }

bool WPlugin::mainWidget() const {
    return getMetaData(Plugin::MainWidget).toBool();
}
void WPlugin::setMainWidget(bool mainWidget) {
    setMetaData(Plugin::MainWidget, mainWidget);
}

bool WPlugin::autorun() const { return getMetaData(Plugin::Autorun).toBool(); }
void WPlugin::setAutorun(bool autorun) {
    setMetaData(Plugin::Autorun, autorun);
}

bool WPlugin::admin() const { return getMetaData(Plugin::Admin).toBool(); }
void WPlugin::setAdmin(bool admin) { setMetaData(Plugin::Admin, admin); }

QStringList WPlugin::depends() const {
    return getMetaData(Plugin::Depends).toStringList();
}
void WPlugin::setDepends(const QStringList &depends) {
    setMetaData(Plugin::Depends, depends);
}

QStringList WPlugin::dependsPath() const {
    return getMetaData(Plugin::DependsPath).toStringList();
}
void WPlugin::setDependsPath(const QStringList &dependsPath) {
    setMetaData(Plugin::DependsPath, dependsPath);
}

QUuid WPlugin::localUuid() const {
    return QUuid(getMetaData(Plugin::LocalUuid).toString());
}
void WPlugin::setLocalUuid(const QUuid &uuid) {
    setMetaData(Plugin::LocalUuid, uuid.toString());
}

QUuid WPlugin::uuid() const {
    return QUuid(getMetaData(Plugin::Uuid).toString());
}
void WPlugin::setUuid(const QUuid &uuid) {
    setMetaData(Plugin::Uuid, uuid.toString());
}

QString WPlugin::configPath() const {
    return getMetaData(Plugin::ConfigPath).toString();
}
void WPlugin::setConfigPath(const QString &configPath) {
    setMetaData(Plugin::ConfigPath, configPath);
}

} // namespace we