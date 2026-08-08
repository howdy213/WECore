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
#include "WECore/config/WConfigDataBase.h"
#include "WECore/config/WConfigRef.h"
#include "WECore/config/WConfigViewer.h"

namespace we::config {

bool WConfigDataBase::revertToPersistent() { return setTemporary(getPersistent()); }

bool WConfigDataBase::revertToDefault() {
    if (m_info.defaultValue().isValid())
        return setTemporary(m_info.defaultValue());
    return false;
}

bool WConfigDataBase::syncPersistent(bool emitSignal) {
    return setPersistent(getTemporary(), emitSignal);
}

bool WConfigDataBase::modified() const { return getTemporary() != getPersistent(); }

bool WConfigDataBase::hasProperty(Property prop) const {
    return m_info.properties().contains(prop);
}

void WConfigDataBase::addObserver(WConfigItemRef *observer) {
    if (observer && !m_observers.contains(observer))
        m_observers.append(observer);
}

void WConfigDataBase::removeObserver(WConfigItemRef *observer) {
    m_observers.removeOne(observer);
}

bool WConfigDataBase::isEffectivelyLocked() const {
    if (isLocked())
        return true;
    WConfigViewer *parentViewer = m_parent;
    while (parentViewer) {
        if (parentViewer->isLocked())
            return true;
        parentViewer = parentViewer->parent();
    }
    return false;
}

QString WConfigDataBase::fullPath() const {
    if (m_parent && !m_parent->name().isEmpty()) {
        return m_parent->fullPath() + "/" + m_key;
    }
    return m_key;
}

void WConfigDataBase::notifyChange() {
    for (auto *obs : std::as_const(m_observers)) {
        if (obs)
            obs->onDataChanged();
    }
}

void WConfigDataBase::notifyDestroy() {
    for (auto *obs : std::as_const(m_observers)) {
        if (obs)
            obs->invalidate();
    }
    m_observers.clear();
}

void WConfigDataBase::init(DataType type, const QString &key,
                           const WConfigItemInfo &info, WConfigViewer *parent) {
    m_type = type;
    m_key = key;
    m_info = info;
    m_parent = parent;
}

} // namespace we::config