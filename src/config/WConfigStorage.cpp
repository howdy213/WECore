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
#include "WECore/config/WConfigStorage.h"
#include "WECore/config/WConfigDataBase.h"
#include "WECore/config/WConfigDocument.h"
#include "WECore/config/WConfigViewer.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QObject>
#include <QSettings>

namespace we::config {

// ---------------------------------------------------------------------------
// File-local helper functions (only use the public API of WConfigDocument / WConfigViewer)
// ---------------------------------------------------------------------------

/// Recursively collect paths of items that are effectively locked and modified; if any exist, saving is refused.
static void collectLockedModified(WConfigViewer *viewer, QStringList &errors) {
    if (!viewer)
        return;
    for (WConfigDataBase *data : viewer->allConfigData()) {
        if (data->isEffectivelyLocked() && data->modified()) {
            errors << data->fullPath();
        }
    }
    for (WConfigViewer *child : viewer->children()) {
        collectLockedModified(child, errors);
    }
}

/// Serialize the file content as indented JSON and write it to disk.
static bool writeJsonFile(const QVariant &variant, const QString &filePath) {
    QDir dir = QFileInfo(filePath).absoluteDir();
    if (!dir.exists() && !dir.mkpath("."))
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    const QByteArray out = QJsonDocument::fromVariant(variant).toJson(QJsonDocument::Indented);
    if (file.write(out) != out.size())
        return false;
    file.flush();
    return file.error() == QFileDevice::NoError;
}

/// Write the content to an Ini file, storing the whole config tree under the "config" key.
static bool writeIniFile(const QVariant &variant, const QString &filePath) {
    QSettings settings(filePath, QSettings::IniFormat);
    settings.setValue("config", variant);
    settings.sync();
    return settings.status() == QSettings::NoError;
}

static void insertNested(QVariantMap &root, const QStringList &path,
                         const QVariant &value) {
    if (path.isEmpty())
        return;
    if (path.size() == 1) {
        root[path.first()] = value;
    } else {
        QString first = path.first();
        QVariantMap subMap = root[first].toMap();
        insertNested(subMap, path.mid(1), value);
        root[first] = subMap;
    }
}

/// Convert all flat keys in QSettings into a nested QVariantMap
static QVariantMap settingsToNestedMap(QSettings *settings) {
    QVariantMap result;
    const QStringList keys = settings->allKeys();
    for (const QString &key : keys) {
        QStringList path = key.split('/', Qt::SkipEmptyParts);
        QVariant value = settings->value(key);
        insertNested(result, path, value);
    }
    return result;
}

/// Recursively flatten a nested QVariantMap and write it into QSettings
static void nestedMapToSettings(const QVariantMap &map, const QString &prefix,
                                QSettings *settings) {
    for (auto it = map.begin(); it != map.end(); ++it) {
        QString key = prefix.isEmpty() ? it.key() : prefix + "/" + it.key();
        if (it.value().typeId() == QMetaType::QVariantMap) {
            nestedMapToSettings(it.value().toMap(), key, settings);
        } else {
            settings->setValue(key, it.value());
        }
    }
}

/// Collect the full paths of all leaf keys in a nested QVariantMap
static void collectLeafKeys(const QVariantMap &map, const QString &prefix,
                            QStringList &keys) {
    for (auto it = map.begin(); it != map.end(); ++it) {
        QString key = prefix.isEmpty() ? it.key() : prefix + "/" + it.key();
        if (it.value().typeId() == QMetaType::QVariantMap) {
            collectLeafKeys(it.value().toMap(), key, keys);
        } else {
            keys.append(key);
        }
    }
}

// ---------------------------------------------------------------------------
// WConfigFileStorage
// ---------------------------------------------------------------------------

WConfigFileStorage::WConfigFileStorage(const QString &filePath)
    : m_filePath(filePath) {}

bool WConfigFileStorage::isReady() const { return !m_filePath.isEmpty(); }

QString WConfigFileStorage::describe() const {
    return m_filePath.isEmpty() ? tr("No config file") : m_filePath;
}

bool WConfigFileStorage::load(WConfigDocument *document) {
    if (!document || !isReady())
        return false;

    const QString suffix = QFileInfo(m_filePath).suffix().toLower();
    if (suffix == "json") {
        QFile file(m_filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "WConfigFileStorage: cannot open file, using defaults:"
                       << m_filePath;
            return false;
        }
        const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isNull()) {
            qWarning() << "WConfigFileStorage: invalid JSON, using defaults:"
                       << m_filePath;
            return false;
        }
        document->loadFromVariant(document->root(), doc.toVariant());
        document->syncToAllPersistent();
        return true;
    }
    if (suffix == "ini") {
        QSettings settings(m_filePath, QSettings::IniFormat);
        // INI and JSON share the same load semantics: both merge and overwrite
        // onto the existing config tree, so items missing from the file keep
        // their template defaults.
        document->loadFromVariant(document->root(), settings.value("config"));
        document->syncToAllPersistent();
        return true;
    }
    qWarning() << "WConfigFileStorage: unsupported config format:" << m_filePath;
    return false;
}

bool WConfigFileStorage::save(WConfigDocument *document, QStringList &errors) {
    errors.clear();
    if (!document || !isReady()) {
        errors << tr("Invalid config file path");
        return false;
    }

    // Items that are locked and modified must not be written, to avoid overwriting externally held data
    collectLockedModified(document->root(), errors);
    if (!errors.isEmpty())
        return false;

    const QVariant variant = document->toVariant();
    const QString suffix = QFileInfo(m_filePath).suffix().toLower();
    if (suffix == "json") {
        if (!writeJsonFile(variant, m_filePath)) {
            errors << tr("Failed to write config file: %1").arg(m_filePath);
            return false;
        }
        return true;
    }
    if (suffix == "ini") {
        if (!writeIniFile(variant, m_filePath)) {
            errors << tr("Failed to write config file: %1").arg(m_filePath);
            return false;
        }
        return true;
    }
    errors << tr("Unsupported config format: %1").arg(m_filePath);
    return false;
}

// ---------------------------------------------------------------------------
// WConfigSettingsStorage
// ---------------------------------------------------------------------------

WConfigSettingsStorage::WConfigSettingsStorage(QSettings *settings)
    : m_settings(settings) {}

bool WConfigSettingsStorage::isReady() const { return m_settings != nullptr; }

QString WConfigSettingsStorage::describe() const {
    return m_settings ? m_settings->fileName() : QString();
}

bool WConfigSettingsStorage::load(WConfigDocument *document) {
    if (!document || !m_settings)
        return false;
    document->loadFromVariant(document->root(), settingsToNestedMap(m_settings));
    document->syncToAllPersistent();
    return true;
}

bool WConfigSettingsStorage::save(WConfigDocument *document,
                                  QStringList &errors) {
    errors.clear();
    if (!document || !m_settings) {
        errors << tr("Invalid QSettings storage");
        return false;
    }

    const QVariantMap nestedMap = document->toVariant().toMap();

    // Collect all keys that the document will write
    QStringList docKeys;
    collectLeafKeys(nestedMap, QString(), docKeys);

    // Remove keys present in QSettings but no longer in the document (persisted deletion)
    const QStringList currentKeys = m_settings->allKeys();
    for (const QString &key : currentKeys) {
        if (!docKeys.contains(key)) {
            m_settings->remove(key);
        }
    }

    // Write all keys of the current document
    nestedMapToSettings(nestedMap, QString(), m_settings);
    m_settings->sync(); // Write immediately
    if (m_settings->status() != QSettings::NoError) {
        errors << tr("Failed to write to QSettings");
        return false;
    }
    return true;
}

} // namespace we::config