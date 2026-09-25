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

WConfig::~WConfig() {
    // Destroying a mounted sub-config: make the host release our root first, so
    // that it is not left dangling inside the host tree.
    if (!m_mountedIn.isNull()) {
        m_mountedIn->unmountSubConfig(this);
    }
    // Hand every mounted subtree back to its owner before our tree is destroyed
    const QList<QPointer<WConfig>> subs = m_subConfigs;
    for (const QPointer<WConfig> &sub : subs) {
        if (sub)
            unmountSubConfig(sub.data());
    }
    m_subConfigs.clear();
}

bool WConfig::initialize(const QString &configFile,
                         WConfigTemplate *configTemplate) {
    if (isMounted()) {
        qWarning() << "WConfig::initialize: a mounted sub-config has no storage of "
                      "its own; the host handles reading and writing";
        return false;
    }
    auto *storage = new WConfigFileStorage(configFile);
    m_storage.reset(storage);
    if (configTemplate) {
        m_document->setTemplate(configTemplate);
    }
    return storage->load(m_document);
}

bool WConfig::initialize(QSettings* settings, WConfigTemplate* configTemplate) {
    if (!settings) return false;
    if (isMounted()) {
        qWarning() << "WConfig::initialize: a mounted sub-config has no storage of "
                      "its own; the host handles reading and writing";
        return false;
    }
    auto *storage = new WConfigSettingsStorage(settings);
    m_storage.reset(storage);
    if (configTemplate) {
        m_document->setTemplate(configTemplate);
    }
    return storage->load(m_document);
}

bool WConfig::applyTemplate(WConfigTemplate *configTemplate) {
    if (!configTemplate)
        return false;
    if (isMounted()) {
        qWarning() << "WConfig::applyTemplate: cannot rebuild the tree of a mounted "
                      "sub-config";
        return false;
    }
    m_document->setTemplate(configTemplate);
    return true;
}

bool WConfig::mountSubConfig(const QString &path, WConfig *sub) {
    if (!sub || sub == this)
        return false;
    if (!sub->m_mountedIn.isNull()) {
        qWarning() << "WConfig::mountSubConfig: sub-config is already mounted";
        return false;
    }
    if (!m_document || !sub->m_document)
        return false;
    if (!m_document->attachMount(path, sub->m_document->root()))
        return false;

    sub->m_mountedIn = this;
    m_subConfigs.append(sub);
    return true;
}

bool WConfig::unmountSubConfig(WConfig *sub) {
    if (!sub)
        return false;
    // Compare raw pointers: `sub` may be inside its own destructor here, so it must
    // not be wrapped into a QPointer again.
    int index = -1;
    for (int i = 0; i < m_subConfigs.size(); ++i) {
        if (m_subConfigs.at(i).data() == sub) {
            index = i;
            break;
        }
    }
    if (index < 0)
        return false;
    m_subConfigs.removeAt(index);
    if (m_document && sub->m_document)
        m_document->detachMount(sub->m_document->root());
    sub->m_mountedIn.clear();
    return true;
}

QList<WConfig *> WConfig::subConfigs() const {
    QList<WConfig *> result;
    for (const QPointer<WConfig> &sub : m_subConfigs) {
        if (sub)
            result.append(sub.data());
    }
    return result;
}

void WConfig::onHostSaved() {
    // The host wrote the whole tree, including this sub-config's items
    m_hasRestartRequiredChanges = false;
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
    QReadLocker locker(lock());
    WConfigDataBase *data = m_document->root()->findConfigData(path);
    return data ? data->getTemporary() : QVariant();
}
bool WConfig::setTemporaryValue(const QString &path, const QVariant &value) {
    QWriteLocker locker(lock());
    WConfigDataBase *data = m_document->root()->findConfigData(path);
    return data ? data->setTemporary(value) : false;
}

QVariant WConfig::getValue(const QString &path) const {
    QReadLocker locker(lock());
    return getValueDirect(path);
}
bool WConfig::setValue(const QString &path, const QVariant &value, bool force) {
    QWriteLocker locker(lock());
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

bool WConfig::setItemProperty(const QString &path, Property prop, bool on) {
    QWriteLocker locker(lock());
    WConfigDataBase *data = m_document->root()->findConfigData(path);
    if (!data)
        return false;
    data->setPropertyRuntime(prop, on);
    emit configChanged(data);
    return true;
}

bool WConfig::setItemReadOnly(const QString &path, bool on) {
    return setItemProperty(path, Property::ReadOnly, on);
}

bool WConfig::setItemRestartRequired(const QString &path, bool on) {
    return setItemProperty(path, Property::RestartRequired, on);
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
    // A mounted sub-config has no storage of its own: the host writes the whole
    // tree, which already contains the mounted subtree. Delegating (instead of
    // the host calling sub->save()) keeps this from recursing.
    if (!m_mountedIn.isNull()) {
        return m_mountedIn->save();
    }
    m_lastSaveErrors.clear();
    if (!m_storage || !m_storage->isReady()) {
        m_lastSaveErrors << tr("No storage backend is configured");
        return false;
    }
    if (!m_storage->save(m_document, m_lastSaveErrors)) {
        return false;
    }
    m_document->syncToAllPersistent();
    m_hasRestartRequiredChanges = false;
    for (const QPointer<WConfig> &sub : m_subConfigs) {
        if (sub)
            sub->onHostSaved();
    }
    return true;
}

bool WConfig::saveAs(const QString &filePath) {
    if (filePath.isEmpty())
        return false;
    m_lastSaveErrors.clear();

    // Save-as always writes to a file, regardless of whether the current backend is QSettings
    WConfigFileStorage target(filePath);
    if (!target.save(m_document, m_lastSaveErrors)) {
        return false;
    }

    // Only switch the target path when the current backend is a file; in QSettings
    // mode save() still writes back to the original QSettings
    if (auto *fileStorage = dynamic_cast<WConfigFileStorage *>(m_storage.get())) {
        fileStorage->setFilePath(filePath);
    }

    m_document->syncToAllPersistent();
    m_hasRestartRequiredChanges = false;
    return true;
}

void WConfig::resetToDefaults() {
    WConfigTemplate *temp = m_document->configTemplate();
    if (!temp)
        return;

    // force=true: a reset overrides locks, then the template is re-applied.
    // clearViewer() clears the content of this tree; for a mounted sub-config the
    // root itself is the mount point and is deliberately kept in place.
    QWriteLocker locker(lock());
    m_document->clearViewer(m_document->root(), true);
    temp->applyTo(m_document->root());
    m_document->syncToAllPersistent();
    // Persist immediately so the reset survives in both file and QSettings modes;
    // for a mounted sub-config this writes through the host.
    if (!save()) {
        qWarning() << "WConfig::resetToDefaults: failed to save after reset"
                   << m_lastSaveErrors;
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