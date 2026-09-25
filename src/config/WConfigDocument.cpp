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
#include "WECore/config/WConfigDocument.h"
#include <QDebug>
#include <QStringList>
#include <QVariantMap>
#include <functional>

namespace we::config {

WConfigDocument::WConfigDocument(QObject *parent)
    : QObject(parent), m_root(new WConfigViewer("")), m_template(nullptr) {}
WConfigDocument::~WConfigDocument() {
    // Mounted nodes belong to another document: unlink them (handing ownership
    // back) before deleting our root, which would otherwise destroy them. Root
    // mounts are merged into m_root, so their content has to be handed back as
    // well.
    const QList<RootMount> rootMounts = m_rootMounts;
    for (const RootMount &mount : rootMounts) {
        detachMount(mount.root);
    }
    const QList<QPair<QString, WConfigViewer *>> mounts = m_mounts;
    for (const QPair<QString, WConfigViewer *> &mount : mounts) {
        detachMount(mount.second);
    }
    delete m_root;
}

bool WConfigDocument::attachMount(const QString &path, WConfigViewer *root) {
    if (!root || !m_root)
        return false;
    if (root == m_root || isMountRoot(root))
        return false;
    if (path.isEmpty())
        return attachRootMount(root);

    const QStringList parts = path.split("/", Qt::SkipEmptyParts);
    if (parts.isEmpty())
        return false;

    // Walk down to the mount point, creating intermediate directories as needed
    WConfigViewer *parent = m_root;
    for (int i = 0; i < parts.size() - 1; ++i) {
        WConfigViewer *next = parent->findOrCreateChild(parts[i]);
        if (!next)
            return false;
        parent = next;
    }

    const QString mountName = parts.last();
    if (parent->getConfigData(mountName)) {
        qWarning() << "WConfigDocument::attachMount: a data item named" << mountName
                   << "already occupies the mount point";
        return false;
    }
    // A directory predeclared at the mount point is replaced by the mounted subtree
    if (WConfigViewer *existing = parent->findChildViewer(mountName)) {
        if (isMountRoot(existing)) {
            qWarning() << "WConfigDocument::attachMount: the mount point" << path
                       << "is already occupied by another mount";
            return false;
        }
        parent->mutableChildren().removeOne(existing);
        delete existing;
    }

    root->setName(mountName);
    root->setParent(parent);
    if (!parent->addChild(root, true))
        return false;

    m_mounts.append(qMakePair(path, root));
    return true;
}

bool WConfigDocument::attachRootMount(WConfigViewer *root) {
    for (const RootMount &mount : std::as_const(m_rootMounts)) {
        if (mount.root == root) {
            qWarning() << "WConfigDocument::attachMount: root mount is already "
                          "attached";
            return false;
        }
    }
    // Refuse the whole mount when a name is taken, instead of merging half of the
    // content and silently dropping the rest.
    for (WConfigViewer *child : root->children()) {
        if (m_root->getConfigData(child->name()) ||
            m_root->findChildViewer(child->name())) {
            qWarning() << "WConfigDocument::attachMount: root mount conflicts with"
                       << child->name();
            return false;
        }
    }
    for (WConfigDataBase *data : root->allConfigData()) {
        if (m_root->getConfigData(data->key()) ||
            m_root->findChildViewer(data->key())) {
            qWarning() << "WConfigDocument::attachMount: root mount conflicts with"
                       << data->key();
            return false;
        }
    }

    // Move the content into the root; the nodes stay owned by the sub-config and
    // are moved back by detachMount().
    RootMount mount;
    mount.root = root;
    const QList<WConfigViewer *> children = root->children();
    for (WConfigViewer *child : children) {
        root->mutableChildren().removeOne(child);
        child->setParent(m_root);
        m_root->mutableChildren().append(child);
        mount.viewers.append(child);
    }
    const QList<WConfigDataBase *> data = root->allConfigData();
    for (WConfigDataBase *item : data) {
        root->mutableConfigData().removeOne(item);
        item->setParent(m_root);
        m_root->mutableConfigData().append(item);
        mount.data.append(item);
    }
    m_rootMounts.append(mount);
    return true;
}

bool WConfigDocument::detachMount(WConfigViewer *root) {
    if (!root)
        return false;
    // Root mount: give every merged node back to the sub-config root
    for (int i = 0; i < m_rootMounts.size(); ++i) {
        if (m_rootMounts.at(i).root != root)
            continue;
        const RootMount mount = m_rootMounts.takeAt(i);
        for (WConfigViewer *child : mount.viewers) {
            m_root->mutableChildren().removeOne(child);
            child->setParent(mount.root);
            mount.root->mutableChildren().append(child);
        }
        for (WConfigDataBase *item : mount.data) {
            m_root->mutableConfigData().removeOne(item);
            item->setParent(mount.root);
            mount.root->mutableConfigData().append(item);
        }
        return true;
    }

    int index = -1;
    for (int i = 0; i < m_mounts.size(); ++i) {
        if (m_mounts.at(i).second == root) {
            index = i;
            break;
        }
    }
    if (index < 0)
        return false;

    // Unlink only; the caller (the owning document) keeps responsibility for the root
    if (WConfigViewer *parent = root->parent()) {
        parent->mutableChildren().removeOne(root);
    }
    root->setName(QString());
    root->setParent(nullptr);
    m_mounts.removeAt(index);
    return true;
}

bool WConfigDocument::isMountRoot(WConfigViewer *viewer) const {
    if (!viewer)
        return false;
    for (const QPair<QString, WConfigViewer *> &mount : m_mounts) {
        if (mount.second == viewer)
            return true;
    }
    // A root mount has no root of its own inside this document: every node it
    // contributed has to be protected instead.
    for (const RootMount &mount : m_rootMounts) {
        if (mount.viewers.contains(viewer))
            return true;
    }
    return false;
}

bool WConfigDocument::isContributedByRootMount(WConfigDataBase *data) const {
    for (const RootMount &mount : m_rootMounts) {
        if (mount.data.contains(data))
            return true;
    }
    return false;
}

void WConfigDocument::setTemplate(WConfigTemplate *configTemplate) {
    m_template = configTemplate;
    if (m_template) {
        clearViewer(m_root);
        m_template->applyTo(m_root);
    }
}

QVariant WConfigDocument::toVariant() const { return saveToVariant(m_root); }

void WConfigDocument::loadFromVariant(WConfigViewer *viewer,
                                      const QVariant &variant) {
    if (!viewer || !variant.canConvert<QVariantMap>())
        return;

    QVariantMap map = variant.toMap();
    WConfigViewer *tmplViewer = nullptr;
    if (m_template) {
        QString fullPath = viewer->fullPath();
        tmplViewer =
            fullPath.isEmpty() ? m_template : m_template->findChildViewer(fullPath);
    }
    bool isRoot = (viewer->parent() == nullptr);

    for (auto it = map.begin(); it != map.end(); ++it) {
        const QString &key = it.key();
        const QVariant &value = it.value();
        if (WConfigDataBase *existing = viewer->getConfigData(key)) {
            existing->fromVariant(value);
            continue;
        }
        // The directory may already exist: the template created it, or a
        // root-mounted sub-config contributed it. Merge into it — adding another
        // child under the same name would be rejected as a duplicate and the
        // stored values would be lost.
        // Note: a custom-type node predeclared by the template is correctly
        // deserialized by the "existing" branch above. If it is serialized as a
        // QVariantMap without being predeclared, it cannot be recognized as a
        // custom type during load and must be predeclared in the template.
        WConfigViewer *sameNameChild = nullptr;
        for (WConfigViewer *child : viewer->children()) {
            if (child->name() == key) {
                sameNameChild = child;
                break;
            }
        }
        if (sameNameChild) {
            loadFromVariant(sameNameChild, value);
            continue;
        }
        bool canCreate = false;
        if ((isRoot && value.canConvert<QVariantMap>()) || m_allowCreateOnLoad) {
            canCreate = true;
        } else {
            canCreate = (viewer->effectiveAcceptPolicy() == AcceptPolicy::ACCEPT);
        }
        WConfigDataBase *tmplData =
            tmplViewer ? tmplViewer->getConfigData(key) : nullptr;
        if (!tmplData && !canCreate)
            continue;
        WConfigDataBase *newData = nullptr;
        if (isRoot && value.canConvert<QVariantMap>()) {
            bool tmplIsObject = (tmplData && tmplData->type() == DataType::Object);
            if (!tmplIsObject) {
                WConfigViewer *newChild = new WConfigViewer(key, viewer);
                if (viewer->addChild(newChild)) {
                    loadFromVariant(newChild, value);
                } else {
                    delete newChild;
                }
                continue;
            }
        }
        if (tmplData) {
            DataType tmplType = tmplData->type();

            if (tmplType == DataType::Object) {
                auto *obj = new WConfigDataObject;
                obj->init(key, tmplData->info(), viewer);
                obj->fromVariant(value);
                newData = obj;
            } else {
                newData =
                    createDataByType(tmplType, key, value, tmplData->info(), viewer);
            }
        } else {
            DataType inferred = inferDataTypeFromVariant(value);
            if (inferred == DataType::None)
                continue;

            if (inferred == DataType::Object) {
                auto *obj = new WConfigDataObject;
                WConfigItemInfo info;
                info.displayName(key);
                obj->init(key, info, viewer);
                obj->fromVariant(value);
                newData = obj;
            } else {
                WConfigItemInfo info;
                info.displayName(key);
                newData = createDataByType(inferred, key, value, info, viewer);
            }
        }
        if (newData) {
            if (!viewer->addConfigData(newData)) {
                delete newData;
                newData = nullptr;
            }
        }
    }
}

QVariant WConfigDocument::saveToVariant(WConfigViewer *viewer) const {
    QVariantMap map;
    // Insert data items before subdirectories so the conflict check in the next loop works
    for (WConfigDataBase *data : viewer->allConfigData()) {
        map[data->key()] = data->toVariant();
    }
    for (WConfigViewer *child : viewer->children()) {
        if (map.contains(child->name())) {
            qWarning() << "WConfigDocument: conflict between data item and "
                          "subdirectory with same name:"
                       << child->name();
            continue;
        }
        map[child->name()] = saveToVariant(child);
    }
    return map;
}

bool WConfigDocument::clearViewer(WConfigViewer *viewer, bool force) {
    if (!viewer)
        return false;
    if (!force)
        if (viewer->isEffectivelyLocked())
            return false;
    // Mounted subtrees belong to another document: skip them so that a template
    // re-apply (setTemplate / resetToDefaults) cannot destroy them. The policy of
    // the mount point itself is still applied by WConfigTemplate::applyPolicyTo().
    const QList<WConfigViewer *> children = viewer->children();
    for (WConfigViewer *child : children) {
        if (isMountRoot(child))
            continue;
        clearViewer(child, force);
        delete child;
        viewer->mutableChildren().removeOne(child);
    }
    // Same for the items a root mount merged in: they cannot be told apart from
    // the viewer's own items, so they are removed one by one instead of clearing
    // the whole list.
    QList<WConfigDataBase *> &data = viewer->mutableConfigData();
    for (int i = data.size() - 1; i >= 0; --i) {
        if (isContributedByRootMount(data.at(i)))
            continue;
        delete data.at(i);
        data.removeAt(i);
    }
    return true;
}

void WConfigDocument::syncToAllPersistent() {
    std::function<void(WConfigViewer *)> traverse = [&](WConfigViewer *viewer) {
        if (!viewer)
            return;
        for (WConfigDataBase *data : viewer->allConfigData()) {
            data->syncPersistentRecursive(true);
        }
        for (WConfigViewer *child : viewer->children()) {
            traverse(child);
        }
    };
    traverse(m_root);
}

} // namespace we::config