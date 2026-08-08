/**
 * @author howdy213
 * @date 2026-08-08
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
#include "WECore/config/WConfig.h"
#include "WECore/config/WConfigDataBase.h"
#include "WECore/config/WConfigDocument.h"
#include "WECore/config/WConfigRef.h"
#include "WECore/config/WConfigViewer.h"
#include <QDebug>

namespace we::config {

WConfig::WConfig(QObject *parent)
    : QObject(parent), m_document(new WConfigDocument(this)),
    m_hasRestartRequiredChanges(false), m_lock(QReadWriteLock::Recursive) {}

WConfig::~WConfig() {}

bool WConfig::initialize(const QString &configFile,
                         WConfigTemplate *configTemplate) {
    m_currentFilePath = configFile;
    if (configTemplate) {
        m_document->setTemplate(configTemplate);
    }
    bool ok = m_document->load(configFile);
    return ok;
}

bool WConfig::initialize(QSettings* settings, WConfigTemplate* configTemplate) {
    if (!settings) return false;
    m_settings = settings;
    m_currentFilePath.clear();  // 清空文件路径，避免混淆
    if (configTemplate) {
        m_document->setTemplate(configTemplate);
    }
    return m_document->loadFromSettings(settings);
}

QVariant WConfig::getValueDirect(const QString &path) const {
    WConfigDataBase *data = m_document->root()->findConfigData(path);
    return data ? data->getPersistent() : QVariant();
}

bool WConfig::setValueDirect(const QString &path, const QVariant &value,
                             bool force) {
    WConfigDataBase *data = m_document->root()->findConfigData(path);
    if (!data)
        return false;
    if (data->hasProperty(Property::ReadOnly))
        return false;
    if (!force)
        if (data->isEffectivelyLocked())
            return false;

    bool changed = data->setPersistent(value, true);
    if (changed && data->hasProperty(Property::RestartRequired)) {
        m_hasRestartRequiredChanges = true;
    }
    return changed;
}

QVariant WConfig::getTemporaryValue(const QString &path) const {
    QReadLocker locker(&m_lock);
    WConfigDataBase *data = m_document->root()->findConfigData(path);
    return data ? data->getTemporary() : QVariant();
}
bool WConfig::setTemporaryValue(const QString &path, const QVariant &value) {
    QWriteLocker locker(&m_lock);
    WConfigDataBase *data = m_document->root()->findConfigData(path);
    return data ? data->setTemporary(value) : false;
}

QVariant WConfig::getValue(const QString &path) const {
    QReadLocker locker(&m_lock);
    return getValueDirect(path);
}
bool WConfig::setValue(const QString &path, const QVariant &value, bool force) {
    QWriteLocker locker(&m_lock);
    return setValueDirect(path, value, force);
}

WConfigItemInfo WConfig::getInfo(const QString &path) const {
    WConfigDataBase *data = m_document->root()->findConfigData(path);
    return data ? data->info() : WConfigItemInfo();
}

bool WConfig::hasProperty(const QString &path, Property prop) const {
    WConfigDataBase *data = m_document->root()->findConfigData(path);
    return data ? data->hasProperty(prop) : false;
}

QSharedPointer<WConfigItemRef> WConfig::createItemRef(const QString &path) {
    WConfigDataBase *data = m_document->root()->findConfigData(path);
    if (!data)
        return nullptr;
    return QSharedPointer<WConfigItemRef>::create(data);
}

QSharedPointer<WConfigDirRef> WConfig::createDirRef(const QString &path) {
    WConfigViewer *viewer = m_document->root()->findChildViewer(path);
    if (!viewer)
        return nullptr;
    return QSharedPointer<WConfigDirRef>::create(viewer);
}

bool WConfig::save() {
    // 优先使用 QSettings 模式
    if (m_settings) {
        m_lastSaveErrors.clear();
        if (!m_document->saveToSettings(m_settings)) {
            m_lastSaveErrors << tr("Failed to write to QSettings");
            return false;
        }
        m_document->syncToAllPersistent();
        m_hasRestartRequiredChanges = false;
        return true;
    }
    // 否则使用文件模式
    if (m_currentFilePath.isEmpty()) return false;
    m_lastSaveErrors.clear();
    if (!m_document->save(m_currentFilePath, m_lastSaveErrors)) {
        return false;
    }
    m_document->syncToAllPersistent();
    m_hasRestartRequiredChanges = false;
    return true;
}

bool WConfig::saveAs(const QString &filePath) {
    if (filePath.isEmpty())
        return false;
    m_lastSaveErrors.clear();

    if (!m_document->save(filePath, m_lastSaveErrors)) {
        return false;
    }

    m_currentFilePath = filePath;
    m_document->syncToAllPersistent();
    m_hasRestartRequiredChanges = false;
    return true;
}

void WConfig::resetToDefaults() {
    WConfigTemplate *temp = m_document->configTemplate();
    if (!temp)
        return;

    // 清除所有现有数据
    QWriteLocker locker(&m_lock);
    m_document->clearViewer(m_document->root(), true);
    temp->applyTo(m_document->root());
    m_document->syncToAllPersistent();
    if (!m_currentFilePath.isEmpty()) {
        m_lastSaveErrors.clear();
        if (!m_document->save(m_currentFilePath, m_lastSaveErrors)) {
            qWarning() << "WConfig::resetToDefaults: failed to save after reset";
        }
    }

    m_hasRestartRequiredChanges = false;
}

bool WConfig::hasRestartRequiredChanges() const {
    return m_hasRestartRequiredChanges;
}

void WConfig::setHasRestartRequiredChanges(bool value) {
    m_hasRestartRequiredChanges = value;
}

QStringList WConfig::lastSaveErrors() const { return m_lastSaveErrors; }

} // namespace we::config