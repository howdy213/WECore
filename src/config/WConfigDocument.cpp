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
#include <QVariantMap>
#include <functional>

namespace we::config {

WConfigDocument::WConfigDocument(QObject *parent)
    : QObject(parent), m_root(new WConfigViewer("")), m_template(nullptr) {}
WConfigDocument::~WConfigDocument() { delete m_root; }

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
        // Note: a custom-type node predeclared by the template is correctly
        // deserialized by the "existing" branch above. If it is serialized as a
        // QVariantMap without being predeclared, it cannot be recognized as a
        // custom type during load and must be predeclared in the template.
        bool isViewerInTemplate = false;
        if (tmplViewer) {
            for (WConfigViewer *child : tmplViewer->children()) {
                if (child->name() == key) {
                    isViewerInTemplate = true;
                    break;
                }
            }
        }
        if (isViewerInTemplate) {
            WConfigViewer *childViewer = viewer->findOrCreateChild(key);
            loadFromVariant(childViewer, value);
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
    if (!force)
        if (viewer->isEffectivelyLocked())
            return false;
    for (WConfigViewer *child : viewer->children()) {
        clearViewer(child, force);
        delete child;
    }
    viewer->mutableChildren().clear();
    for (WConfigDataBase *data : viewer->allConfigData()) {
        delete data;
    }
    viewer->mutableConfigData().clear();
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