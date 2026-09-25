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
#include "WECore/config/WConfigViewer.h"
#include <QStringList>

namespace we::config {

WConfigViewer::WConfigViewer(const QString &name, WConfigViewer *parent)
    : m_name(name), m_parent(parent) {}
WConfigViewer::~WConfigViewer() {
    qDeleteAll(m_children);
    qDeleteAll(m_configData);
    qDeleteAll(m_groups);
}

WConfigGroupSpec *WConfigViewer::addGroup(const WConfigGroupSpec &spec) {
    if (spec.groupId.isEmpty())
        return nullptr;
    for (WConfigGroupSpec *g : std::as_const(m_groups)) {
        if (g->groupId == spec.groupId)
            return nullptr; // reject a duplicate id
    }
    auto *owned = new WConfigGroupSpec(spec);
    m_groups.append(owned);
    return owned;
}

bool WConfigViewer::removeGroup(const QString &groupId) {
    for (int i = 0; i < m_groups.size(); ++i) {
        if (m_groups[i]->groupId == groupId) {
            delete m_groups.takeAt(i);
            return true;
        }
    }
    return false;
}

WConfigGroupSpec *WConfigViewer::groupById(const QString &groupId) const {
    for (WConfigGroupSpec *g : std::as_const(m_groups)) {
        if (g->groupId == groupId)
            return g;
    }
    return nullptr;
}

void WConfigViewer::clearGroups() {
    qDeleteAll(m_groups);
    m_groups.clear();
}

void WConfigViewer::setGroups(const QList<WConfigGroupSpec *> &in) {
    // Replace existing groups: free the old ones, then take ownership of the new
    // pointers (the destructor frees them as well, so the caller must not keep them)
    qDeleteAll(m_groups);
    m_groups = in;
}

QString WConfigViewer::fullPath() const {
    if (m_parent && !m_parent->name().isEmpty())
        return m_parent->fullPath() + "/" + m_name;
    return m_name;
}

WConfigViewer *WConfigViewer::findChildViewer(const QString &path) {
    QStringList parts = path.split("/", Qt::SkipEmptyParts);
    if (parts.isEmpty())
        return nullptr;
    WConfigViewer *current = this;
    for (const QString &part : std::as_const(parts)) {
        bool found = false;
        for (WConfigViewer *child : std::as_const(current->m_children)) {
            if (child->name() == part) {
                current = child;
                found = true;
                break;
            }
        }
        if (!found)
            return nullptr;
    }
    return current;
}

WConfigDataBase *WConfigViewer::findConfigData(const QString &path) {
    QStringList parts = path.split("/", Qt::SkipEmptyParts);
    if (parts.isEmpty())
        return nullptr;
    // All segments but the last must be directories; the last is a data-item key
    WConfigViewer *current = this;
    for (int i = 0; i < parts.size(); ++i) {
        const QString &key = parts[i];
        if (i == parts.size() - 1) {
            return current->getConfigData(key);
        } else {
            WConfigViewer *child = nullptr;
            for (WConfigViewer *c : std::as_const(current->m_children)) {
                if (c->name() == key) {
                    child = c;
                    break;
                }
            }
            if (!child)
                return nullptr;
            current = child;
        }
    }
    return nullptr;
}

bool WConfigViewer::addChild(WConfigViewer *child, bool force) {
    if (!child)
        return false;
    if(!force)if(child->isEffectivelyLocked())return false;
    for (WConfigViewer *c : m_children) {
        if (c->name() == child->name()) {
            qWarning() << "addChild: child with name" << child->name()
                       << "already exists";
            return false;
        }
    }
    if (getConfigData(child->name())) {
        qWarning() << "addChild: a data item with name" << child->name()
                   << "already exists";
        return false;
    }
    m_children.append(child);
    return true;
}

bool WConfigViewer::removeChild(WConfigViewer* child, bool force) {
    if (!child || !m_children.contains(child))
        return false;
    if (!force && isEffectivelyLocked()) {
        qWarning() << "removeChild: viewer is locked";
        return false;
    }
    m_children.removeOne(child);
    delete child;
    return true;
}

bool WConfigViewer::addConfigData(WConfigDataBase *data, bool force) {
    if (!data)
        return false;
    if(!force)if(data->isEffectivelyLocked())return false;
    if (getConfigData(data->key())) {
        qWarning() << "addConfigData: data item with key" << data->key()
                   << "already exists";
        return false;
    }
    for (WConfigViewer *child : m_children) {
        if (child->name() == data->key()) {
            qWarning() << "addConfigData: a child viewer with name" << data->key()
                       << "already exists";
            return false;
        }
    }
    data->setParent(this);
    m_configData.append(data);
    return true;
}
bool WConfigViewer::removeConfigData(WConfigDataBase *data, bool force) {
    if (!data)
        return false;
    if(!force)if(data->isEffectivelyLocked())return false;
    return m_configData.removeOne(data);
}

bool WConfigViewer::clearAll(bool force) {
    if(!force)if(this->isEffectivelyLocked())return false;
    qDeleteAll(m_children);
    m_children.clear();
    qDeleteAll(m_configData);
    m_configData.clear();
    return true;
}

WConfigDataBase *WConfigViewer::getConfigData(const QString &key) const {
    for (WConfigDataBase *data : m_configData)
        if (data->key() == key)
            return data;
    return nullptr;
}

WConfigViewer *WConfigViewer::findOrCreateChild(const QString &name) {
    for (WConfigViewer *child : std::as_const(m_children))
        if (child->name() == name)
            return child;
    WConfigViewer *newChild = new WConfigViewer(name, this);
    if (!addChild(newChild)) {
        delete newChild;
        newChild = nullptr;
    }
    return newChild;
}

void WConfigViewer::setDirectoryPolicy(AcceptPolicy policy,
                                       PolicyPropagation propagation,
                                       OverwriteMode overwrite,
                                       bool createIfNotExist) {
    Q_UNUSED(createIfNotExist);
    if (propagation == PolicyPropagation::ApplyToSelfOnly) {
        m_acceptPolicy = policy;
        return;
    }
    std::function<void(WConfigViewer *)> apply = [&](WConfigViewer *node) {
        if (overwrite == OverwriteMode::ForceOverwrite ||
            node->m_acceptPolicy == AcceptPolicy::DEFAULT) {
            node->m_acceptPolicy = policy;
        }
        for (WConfigViewer *child : std::as_const(node->m_children))
            apply(child);
    };
    apply(this);
}
AcceptPolicy WConfigViewer::effectiveAcceptPolicy() const {
    if (m_acceptPolicy != AcceptPolicy::DEFAULT)
        return m_acceptPolicy;
    if (m_parent)
        return m_parent->effectiveAcceptPolicy();
    return AcceptPolicy::REFUSE; // the root refuses by default
}
bool WConfigViewer::isEffectivelyLocked() const {
    if (isLocked())
        return true;
    if (m_parent)
        return m_parent->isEffectivelyLocked();
    return false;
}
} // namespace we::config