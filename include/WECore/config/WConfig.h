/**
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
#ifndef WCONFIG_H
#define WCONFIG_H

#include "WConfigDocument.h"
#include "WConfigStorage.h"
#include "WECore/def/wedef.h"
#include <QList>
#include <QObject>
#include <QPointer>
#include <QReadWriteLock>
#include <QSettings>
#include <memory>

namespace we::config {

/**
 * @brief Thread-safe entry point for reading and writing a configuration tree.
 *
 * Owns a WConfigDocument (in-memory tree) plus one storage backend (file or
 * QSettings) chosen by initialize(). The non-Direct getters/setters take a
 * recursive read/write lock internally, so they may be called from any thread;
 * the *Direct variants assume the caller already holds the lock or runs
 * single-threaded.
 */
class WE_EXPORT WConfig : public QObject {
    Q_OBJECT
public:
    explicit WConfig(QObject *parent = nullptr);
    ~WConfig() override;

    bool initialize(const QString &configFile,
                    WConfigTemplate *configTemplate = nullptr);
    bool initialize(QSettings *settings,
                    WConfigTemplate *configTemplate = nullptr);
    /// Build the in-memory tree from a template without configuring a storage
    /// backend. Used by sub-configs, which are mounted into a host and share its
    /// storage instead of owning one.
    bool applyTemplate(WConfigTemplate *configTemplate);
    WConfigDocument *document() const { return m_document; }

    // ---- Sub-config mounting ----
    // A sub-config contributes its own tree to `path` inside this config's tree.
    // An empty `path` merges the whole tree into the root of this config instead,
    // which keeps a flat file layout (used when the entire file is described by a
    // sub-config). Ownership of the sub stays with the caller; this config never
    // deletes it.
    // mountSubConfig() must be called BEFORE initialize(), so the host template
    // only has to declare the mount point itself (see WConfigDocument::attachMount).
    bool mountSubConfig(const QString &path, WConfig *sub);
    bool unmountSubConfig(WConfig *sub);
    QList<WConfig *> subConfigs() const;
    /// Whether this config is currently mounted into another one.
    bool isMounted() const { return !m_mountedIn.isNull(); }
    /// The config this one is mounted into, or nullptr when standalone.
    WConfig *mountedIn() const { return m_mountedIn.data(); }

    // Direct variants take no read/write lock, for callers that already hold
    // the lock or run single-threaded. Non-Direct variants acquire m_lock
    // internally and can be called directly from any thread.
    QVariant getValueDirect(const QString &path) const;
    bool setValueDirect(const QString &path, const QVariant &value,
                        bool force = false);
    QVariant getTemporaryValue(const QString &path) const;
    bool setTemporaryValue(const QString &path, const QVariant &value);
    QVariant getValue(const QString &path) const;
    bool setValue(const QString &path, const QVariant &value, bool force = false);

    WConfigItemInfo getInfo(const QString &path) const;
    bool hasProperty(const QString &path, Property prop) const;
    // Set/clear a property at runtime; triggers data->setPropertyRuntime + configChanged
    bool setItemProperty(const QString &path, Property prop, bool on);
    bool setItemReadOnly(const QString &path, bool on);
    bool setItemRestartRequired(const QString &path, bool on);

    // Root document lock: used for cross-thread config item access (recursive).
    // A mounted sub-config shares its host's lock, so that locking the mount path
    // and locking the sub directly go through the same lock.
    QReadWriteLock *lock() const {
        return m_mountedIn.isNull() ? &m_lock : m_mountedIn->lock();
    }
    QSharedPointer<WConfigItemRef> createItemRef(const QString &path);
    QSharedPointer<WConfigDirRef> createDirRef(const QString &path);

    bool save();                          // writes through the configured backend (file or QSettings)
    bool saveAs(const QString &filePath); // always writes to a file
    QStringList lastSaveErrors() const;
    void resetToDefaults();

    bool hasRestartRequiredChanges() const;
    void setHasRestartRequiredChanges(bool value);

    operator QReadWriteLock *() const { return lock(); }
signals:
    void configChanged(we::config::WConfigDataBase *data);

private:
    /// Invoked by the host after it wrote the whole tree: a mounted sub-config has
    /// no write of its own, so it only clears its restart flag here.
    void onHostSaved();

    /// Current storage backend (file or QSettings), chosen by initialize()
    std::unique_ptr<WConfigStorage> m_storage;
    WConfigDocument *m_document = nullptr;
    bool m_hasRestartRequiredChanges;
    QStringList m_lastSaveErrors;
    mutable QReadWriteLock m_lock;

    /// Host this config is mounted into (null when standalone). Ownership of the
    /// root is implied: a mounted config's root lives in the host tree, and the
    /// host always detaches it before its own tree is destroyed.
    QPointer<WConfig> m_mountedIn;
    /// Sub-configs mounted into this host (not owned)
    QList<QPointer<WConfig>> m_subConfigs;
};

} // namespace we::config

#endif // WCONFIG_H