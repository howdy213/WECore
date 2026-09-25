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
#ifndef WCONFIGSTORAGE_H
#define WCONFIGSTORAGE_H

#include "WECore/def/wedef.h"
#include <QCoreApplication>
#include <QObject>
#include <QString>
#include <QStringList>

class QSettings;

namespace we::config {

class WConfigDocument;

/**
 * @brief Abstract interface for a configuration storage backend.
 *
 * Separates "where to read from / write to" from WConfig / WConfigDocument:
 * WConfigDocument only owns the in-memory config tree, while WConfigStorage
 * handles reading/writing against a file or QSettings. See WConfigFileStorage
 * and WConfigSettingsStorage for the concrete implementations.
 */
class WE_EXPORT WConfigStorage {
public:
    Q_DECLARE_TR_FUNCTIONS(WConfigStorage)

public:
    virtual ~WConfigStorage() = default;

    /**
     * @brief Read configuration from the backend and merge it into the document.
     * @return true on success; false if the backend is not ready or the data is
     *         unavailable, in which case the document is left unchanged
     *         (usually the template defaults).
     */
    virtual bool load(WConfigDocument *document) = 0;

    /**
     * @brief Write the document content to the backend.
     * @param errors Output parameter; on failure, receives the failure reason
     *               or the paths of items that refused to be saved.
     * @return true on success.
     */
    virtual bool save(WConfigDocument *document, QStringList &errors) = 0;

    /// Whether the backend is usable for reading/writing (path/settings object configured).
    virtual bool isReady() const = 0;

    /// Human-readable description of the backend, used in logs and error messages.
    virtual QString describe() const = 0;
};

/**
 * @brief Storage backend based on a configuration file (JSON / Ini).
 *
 * Dispatches read/write by file extension: json uses QJsonDocument, ini uses
 * QSettings::IniFormat and stores the whole config tree under the "config" key.
 */
class WE_EXPORT WConfigFileStorage : public WConfigStorage {
public:
    Q_DECLARE_TR_FUNCTIONS(WConfigFileStorage)

public:
    explicit WConfigFileStorage(const QString &filePath = QString());

    /// Change the target file; an empty path means the backend is not ready.
    void setFilePath(const QString &filePath) { m_filePath = filePath; }
    QString filePath() const { return m_filePath; }

    bool load(WConfigDocument *document) override;
    bool save(WConfigDocument *document, QStringList &errors) override;
    bool isReady() const override;
    QString describe() const override;

private:
    QString m_filePath;
};

/**
 * @brief Storage backend based on an external QSettings.
 *
 * QSettings is owned by the caller; this class only uses it during its
 * lifetime and does not free it. Because QSettings is a flat key/value
 * structure, reads/writes convert between nested maps and "a/b/c"-style keys,
 * and stale keys no longer present in the document are cleaned up.
 */
class WE_EXPORT WConfigSettingsStorage : public WConfigStorage {
public:
    Q_DECLARE_TR_FUNCTIONS(WConfigSettingsStorage)

public:
    explicit WConfigSettingsStorage(QSettings *settings = nullptr);

    QSettings *settings() const { return m_settings; }

    bool load(WConfigDocument *document) override;
    bool save(WConfigDocument *document, QStringList &errors) override;
    bool isReady() const override;
    QString describe() const override;

private:
    QSettings *m_settings = nullptr; ///< Not owned; the caller guarantees its lifetime
};

} // namespace we::config

#endif // WCONFIGSTORAGE_H