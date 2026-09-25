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
#include "WECore/config/WConfigRef.h"
#include "WECore/config/WConfigDataBase.h"
#include "WECore/config/WConfigDef.h"
#include "WECore/config/WConfigViewer.h"
#include <QDebug>

namespace we::config {

WConfigItemRef::WConfigItemRef(WConfigDataBase *data) : m_data(data) {
    if (m_data)
        m_data->addObserver(this);
}

WConfigItemRef::~WConfigItemRef() {
    if (m_data)
        m_data->removeObserver(this);
}

QVariant WConfigItemRef::value() const {
    return m_data ? m_data->getTemporary() : QVariant();
}

bool WConfigItemRef::setValue(const QVariant &val, bool force) {
    if (!force)
        if (isEffectivelyLocked())
            return false;
    return m_data ? m_data->setTemporary(val) : false;
}

void WConfigItemRef::lock() {
    if (m_data)
        m_data->lock();
}

void WConfigItemRef::unlock() {
    if (m_data)
        m_data->unlock();
}

bool WConfigItemRef::isLocked() const {
    return m_data ? m_data->isLocked() : false;
}

bool WConfigItemRef::isEffectivelyLocked() const {
    return m_data ? m_data->isEffectivelyLocked() : false;
}

QString WConfigItemRef::path() const {
    return m_data ? m_data->fullPath() : QString();
}

void WConfigItemRef::setOnChanged(std::function<void()> callback) {
    m_onChanged = callback;
}

void WConfigItemRef::onDataChanged() {
    if (m_onChanged)
        m_onChanged();
}

void WConfigItemRef::invalidate() {
    m_data = nullptr;
    m_onChanged = nullptr;
}

// ==================== WConfigDirRef ====================

WConfigDirRef::WConfigDirRef(WConfigViewer *viewer) : m_viewer(viewer) {}

WConfigDirRef::~WConfigDirRef() {}

QVariantMap WConfigDirRef::toMap() const {
    QVariantMap map;
    if (!m_viewer)
        return map;
    for (auto *data : m_viewer->allConfigData()) {
        map[data->key()] = data->getTemporary(); // current level only, no recursion
    }
    return map;
}

bool WConfigDirRef::fromMap(const QVariantMap &map, bool force) {
    if (!m_viewer)
        return false;
    if (!force)
        if (isEffectivelyLocked())
            return false;
    bool ok = true;
    for (auto it = map.begin(); it != map.end(); ++it) {
        WConfigDataBase *data = m_viewer->getConfigData(it.key());
        if (data) {
            if (!data->setTemporary(it.value()))
                ok = false;
        } else {
            qWarning() << "WConfigDirRef::fromMap: key not found" << it.key();
            ok = false;
        }
    }
    return ok;
}

QVariant WConfigDirRef::getRelative(const QString &relPath) const {
    if (!m_viewer || !validateRelPath(relPath))
        return QVariant();
    if (relPath.isEmpty())
        return toVariant(); // empty path: return the whole directory recursively

    QStringList parts = relPath.split('/', Qt::SkipEmptyParts);
    WConfigViewer *current = m_viewer;
    for (int i = 0; i < parts.size(); ++i) {
        const QString &part = parts[i];
        if (i == parts.size() - 1) {
            // Last segment: a data item takes precedence over a subdirectory
            WConfigDataBase *data = current->getConfigData(part);
            if (data)
                return data->getTemporary();
            WConfigViewer *child = current->findChildViewer(part);
            if (child) {
                WConfigDirRef childRef(child);
                return childRef.toVariant();
            }
            return QVariant();
        } else {
            // Intermediate segment: descend into the subdirectory
            WConfigViewer *child = current->findChildViewer(part);
            if (!child)
                return QVariant();
            current = child;
        }
    }
    return QVariant();
}

bool WConfigDirRef::setRelative(const QString &relPath, const QVariant &val,
                                bool force) {
    if (!m_viewer || !validateRelPath(relPath))
        return false;
    if (relPath.isEmpty())
        return false; // the root directory itself cannot be set
    if (!force)
        if (isEffectivelyLocked())
            return false;
    QStringList parts = relPath.split('/', Qt::SkipEmptyParts);
    WConfigViewer *current = m_viewer;
    for (int i = 0; i < parts.size(); ++i) {
        const QString &part = parts[i];
        if (i == parts.size() - 1) {
            // Last segment: a data item takes precedence over a subdirectory
            WConfigDataBase *data = current->getConfigData(part);
            if (data) {
                return data->setTemporary(val);
            }
            WConfigViewer *child = current->findChildViewer(part);
            if (child) {
                WConfigDirRef childRef(child);
                return childRef.fromVariant(val);
            }
            // Not found: create it, as a subdirectory for a map value, else as a data item
            if (val.canConvert<QVariantMap>()) {
                WConfigViewer *newChild = new WConfigViewer(part, current);
                current->addChild(newChild, force);
                WConfigDirRef newRef(newChild);
                return newRef.fromVariant(val);
            } else {
                WConfigDataBase *newData = createDataFromVariant(part, val, current);
                if (newData) {
                    if (!current->addConfigData(newData, force)) {
                        delete newData;
                        return false;
                    }
                    return true;
                }
                return false;
            }
        } else {
            // Intermediate segment: must exist or be created as a directory
            WConfigViewer *child = current->findChildViewer(part);
            if (!child) {
                child = new WConfigViewer(part, current);
                current->addChild(child, force);
            }
            current = child;
        }
    }
    return false;
}

void WConfigDirRef::lock() {
    if (m_viewer)
        m_viewer->lock();
}

void WConfigDirRef::unlock() {
    if (m_viewer)
        m_viewer->unlock();
}

bool WConfigDirRef::isLocked() const {
    return m_viewer ? m_viewer->isLocked() : false;
}

bool WConfigDirRef::isEffectivelyLocked() const {
    return m_viewer ? m_viewer->isEffectivelyLocked() : false;
}

QString WConfigDirRef::path() const {
    return m_viewer ? m_viewer->fullPath() : QString();
}

QVariant WConfigDirRef::toVariant() const {
    if (!m_viewer)
        return QVariantMap();
    QVariantMap map;
    for (auto *data : m_viewer->allConfigData()) {
        map[data->key()] = data->toVariant();
    }
    for (auto *child : m_viewer->children()) {
        WConfigDirRef childRef(child);
        map[child->name()] = childRef.toVariant();
    }
    return map;
}

bool WConfigDirRef::fromVariant(const QVariant &variant, bool force) {
    if (!m_viewer || !variant.canConvert<QVariantMap>())
        return false;
    if (!force)
        if (isEffectivelyLocked())
            return false;
    QVariantMap map = variant.toMap();

    // Update existing items and subdirectories first, then create the missing ones
    for (auto it = map.begin(); it != map.end(); ++it) {
        const QString &key = it.key();
        const QVariant &value = it.value();

        WConfigDataBase *data = m_viewer->getConfigData(key);
        if (data) {
            data->fromVariant(value);
            continue;
        }

        WConfigViewer *child = m_viewer->findChildViewer(key);
        if (child) {
            WConfigDirRef childRef(child);
            childRef.fromVariant(value);
            continue;
        }

        // Not present: create it
        if (value.canConvert<QVariantMap>()) {
            WConfigViewer *newChild = new WConfigViewer(key, m_viewer);
            m_viewer->addChild(newChild);
            WConfigDirRef newRef(newChild);
            newRef.fromVariant(value);
        } else {
            WConfigDataBase *newData = createDataFromVariant(key, value, m_viewer);
            if (newData) {
                m_viewer->addConfigData(newData, force);
            }
        }
    }
    return true;
}

QStringList WConfigDirRef::childKeys() const {
    QStringList keys;
    if (!m_viewer)
        return keys;
    for (auto *data : m_viewer->allConfigData()) {
        keys << data->key();
    }
    for (auto *child : m_viewer->children()) {
        keys << child->name();
    }
    return keys;
}

bool WConfigDirRef::contains(const QString &relPath) const {
    if (!m_viewer || !validateRelPath(relPath))
        return false;
    if (relPath.isEmpty())
        return true; // the current directory always exists

    QStringList parts = relPath.split('/', Qt::SkipEmptyParts);
    WConfigViewer *current = m_viewer;
    for (int i = 0; i < parts.size(); ++i) {
        const QString &part = parts[i];
        if (i == parts.size() - 1) {
            if (current->getConfigData(part))
                return true;
            if (current->findChildViewer(part))
                return true;
            return false;
        } else {
            WConfigViewer *child = current->findChildViewer(part);
            if (!child)
                return false;
            current = child;
        }
    }
    return false;
}

DataType WConfigDirRef::typeOf(const QString &relPath) const {
    if (!m_viewer || !validateRelPath(relPath))
        return DataType::None;
    if (relPath.isEmpty())
        return DataType::Object; // the current directory is treated as Object

    QStringList parts = relPath.split('/', Qt::SkipEmptyParts);
    WConfigViewer *current = m_viewer;
    for (int i = 0; i < parts.size(); ++i) {
        const QString &part = parts[i];
        if (i == parts.size() - 1) {
            WConfigDataBase *data = current->getConfigData(part);
            if (data)
                return data->type();
            if (current->findChildViewer(part))
                return DataType::Object;
            return DataType::None;
        } else {
            WConfigViewer *child = current->findChildViewer(part);
            if (!child)
                return DataType::None;
            current = child;
        }
    }
    return DataType::None;
}

QSharedPointer<WConfigDirRef> WConfigDirRef::subDir(const QString &name) const {
    if (!m_viewer)
        return nullptr;
    WConfigViewer *child = m_viewer->findChildViewer(name);
    if (!child)
        return nullptr;
    return QSharedPointer<WConfigDirRef>::create(child);
}

bool WConfigDirRef::validateRelPath(const QString &relPath) const {
    if (relPath.isEmpty())
        return true;
    QStringList parts = relPath.split('/', Qt::SkipEmptyParts);
    for (const QString &p : std::as_const(parts)) {
        if (p == "..")
            return false;
    }
    return true;
}

} // namespace we::config