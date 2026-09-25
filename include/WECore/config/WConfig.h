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
#include <QObject>
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
    WConfigDocument *document() const { return m_document; }

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

    // Root document lock: used for cross-thread config item access (recursive)
    QReadWriteLock *lock() const { return &m_lock; }
    QSharedPointer<WConfigItemRef> createItemRef(const QString &path);
    QSharedPointer<WConfigDirRef> createDirRef(const QString &path);

    bool save();                          // writes through the configured backend (file or QSettings)
    bool saveAs(const QString &filePath); // always writes to a file
    QStringList lastSaveErrors() const;
    void resetToDefaults();

    bool hasRestartRequiredChanges() const;
    void setHasRestartRequiredChanges(bool value);

    operator QReadWriteLock *() const { return &m_lock; }
signals:
    void configChanged(we::config::WConfigDataBase *data);

private:
    /// Current storage backend (file or QSettings), chosen by initialize()
    std::unique_ptr<WConfigStorage> m_storage;
    WConfigDocument *m_document = nullptr;
    bool m_hasRestartRequiredChanges;
    QStringList m_lastSaveErrors;
    mutable QReadWriteLock m_lock;
};

} // namespace we::config

#endif // WCONFIG_H