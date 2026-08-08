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
#include "WECore/config/WConfigDataObject.h"
#include "WECore/config/WConfigDataDef.h"
#include <QDebug>
#include <QMetaType>

namespace we::config {

WConfigDataObject::WConfigDataObject() { m_type = DataType::Object; }

WConfigDataBase *WConfigDataObject::init(const QString &key,
                                         const WConfigItemInfo &info,
                                         WConfigViewer *parent) {
    WConfigDataBase::init(DataType::Object, key, info, parent);
    if (!m_content)
        m_content = new WConfigViewer(key, nullptr);

    // 如果 info 有默认值且是 Map，则应用它
    if (info.defaultValue().canConvert<QVariantMap>()) {
        fromVariant(info.defaultValue());
    }

    // 如果没有有效的默认值，但有子项，则自动构建默认值
    if (!m_info.defaultValue().isValid() &&
        !m_content->allConfigData().isEmpty()) {
        buildDefaultMap();
    }

    m_originalMap = toVariant().toMap();
    return this;
}

WConfigDataObject *WConfigDataObject::init(const QString &key,
                                           const QVariantMap &defaultMap,
                                           const Properties &properties,
                                           WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultMap);
    for (Property p : properties)
        info.property(p);
    init(key, info, parent);
    return this;
}

WConfigDataObject::~WConfigDataObject() { delete m_content; }

bool WConfigDataObject::setTemporary(const QVariant &value) {
    QVariant oldVal = toVariant();
    bool ok = fromVariant(value);
    return ok;
}

bool WConfigDataObject::setPersistent(const QVariant &val, bool emitSignal) {
    if (!val.canConvert<QVariantMap>())
        return false;
    QVariantMap newMap = val.toMap();
    if (m_originalMap == newMap)
        return false;
    m_originalMap = newMap;
    if (emitSignal)
        notifyChange();
    return true;
}

bool WConfigDataObject::revertToPersistent() {
    m_content->clearAll();
    return fromVariant(m_originalMap);
}

QVariant WConfigDataObject::toVariant() const {
    QVariantMap map;
    for (WConfigDataBase *data : m_content->allConfigData()) {
        map[data->key()] = data->toVariant();
    }
    return map;
}
bool WConfigDataObject::fromVariant(const QVariant &variant) {
    if (!variant.canConvert<QVariantMap>())
        return false;

    QVariantMap map = variant.toMap();
    QStringList keys = map.keys();

    QList<WConfigDataBase *> toRemove;
    for (WConfigDataBase *child : m_content->allConfigData()) {
        if (!keys.contains(child->key())) {
            toRemove.append(child);
        }
    }
    for (WConfigDataBase *child : toRemove) {
        if (child->isFromTemplate())
            continue;
        m_content->removeConfigData(child);
        delete child;
    }
    for (auto it = map.begin(); it != map.end(); ++it) {
        const QString &key = it.key();
        const QVariant &value = it.value();

        WConfigDataBase *existing = findChildData(key);
        if (existing) {
            existing->fromVariant(value);
        } else {
            WConfigDataBase *newData = createDataFromVariant(key, value, m_content);
            if (newData) {
                if (!m_content->addConfigData(newData)) {
                    delete newData;
                    newData = nullptr;
                }
            }
        }
    }

    return true;
}

void WConfigDataObject::buildDefaultMap() {
    QVariantMap defaultMap = buildDefaultMapFromContent();
    m_info.defaultValue(defaultMap);
}

void WConfigDataObject::forEachChild(
    std::function<void(WConfigDataBase *)> func) const {
    for (WConfigDataBase *data : m_content->allConfigData()) {
        func(data);
        data->forEachChild(func);
    }
}

void WConfigDataObject::syncPersistentRecursive(bool emitSignal) {
    syncPersistent();
    for (WConfigDataBase *data : m_content->allConfigData()) {
        data->syncPersistentRecursive(emitSignal);
    }
}

void WConfigDataObject::revertToPersistentRecursive() {
    revertToPersistent();
    for (WConfigDataBase *data : m_content->allConfigData()) {
        data->revertToPersistentRecursive();
    }
}

bool WConfigDataObject::addChildData(WConfigDataBase *data,
                                     bool isFromTemplate) {
    if (!data)
        return false;
    if (findChildData(data->key())) {
        qDebug() << "Object item exists!";
        return false;
    }
    for (WConfigViewer *child : m_content->children()) {
        if (child->name() == data->key()) {
            qWarning() << "addChildData: a child viewer with same name exists";
            return false;
        }
    }

    data->setIsFromTemplate(isFromTemplate);
    data->setParent(m_content);
    return m_content->addConfigData(data);
}

WConfigDataBase *WConfigDataObject::findChildData(const QString &key) const {
    return m_content ? m_content->getConfigData(key) : nullptr;
}

QVariantMap WConfigDataObject::buildDefaultMapFromContent() const {
    QVariantMap map;
    for (const WConfigDataBase *child : m_content->allConfigData()) {
        if (!child->isFromTemplate())
            continue;
        if (child->type() == DataType::Object) {
            auto *childObj = static_cast<const WConfigDataObject *>(child);
            map[child->key()] = childObj->buildDefaultMapFromContent();
        } else {
            QVariant defaultVal = child->info().defaultValue();
            if (!defaultVal.isValid()) {
                defaultVal = defaultVariantForType(child->type());
            }
            map[child->key()] = defaultVal;
        }
    }
    return map;
}

} // namespace we::config