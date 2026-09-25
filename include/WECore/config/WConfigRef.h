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
#ifndef WCONFIGREF_H
#define WCONFIGREF_H

#include "WConfigDef.h"
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <functional>

namespace we::config {

class WConfigDataBase;
class WConfigViewer;

// ---------- Config item reference ----------
// Non-owning handle to a single WConfigDataBase; registers itself as an observer
// of that item. If the item may be destroyed while the ref is alive, call
// invalidate() first, otherwise the ref would dangle.
class WE_EXPORT WConfigItemRef {
public:
    explicit WConfigItemRef(WConfigDataBase *data);
    ~WConfigItemRef();

    QVariant value() const;
    bool setValue(const QVariant &val, bool force = false);

    void lock();
    void unlock();
    bool isLocked() const;
    bool isEffectivelyLocked() const;
    QString path() const;

    void setOnChanged(std::function<void()> callback);
    void onDataChanged();
    void invalidate();

private:
    WConfigDataBase *m_data;
    std::function<void()> m_onChanged;
};

// ---------- Directory reference ----------
// Non-owning handle to a WConfigViewer subtree; all relative paths are resolved
// against that directory.
class WE_EXPORT WConfigDirRef {
public:
    explicit WConfigDirRef(WConfigViewer *viewer);
    ~WConfigDirRef();
    // Current level's data items only
    QVariantMap toMap() const;
    // Updates only the data items already present at the current level
    bool fromMap(const QVariantMap &map, bool force = false);
    // Accepts multi-level paths; returns the item value or the subtree as a QVariant
    QVariant getRelative(const QString &relPath) const;
    // Accepts multi-level paths; creates intermediate directories as needed
    bool setRelative(const QString &relPath, const QVariant &val,
                     bool force = false);

    void lock();
    void unlock();
    bool isLocked() const;
    bool isEffectivelyLocked() const;
    QString path() const;
    // Recursively read the whole subtree (including subdirectories)
    QVariant toVariant() const;
    // Recursively restore the whole subtree (may create new items/directories)
    bool fromVariant(const QVariant &variant, bool force = false);
    // Names of all children (data items + subdirectories) under this directory
    QStringList childKeys() const;
    // Whether the relative path exists
    bool contains(const QString &relPath) const;
    // Data type at the relative path (Object for a directory)
    DataType typeOf(const QString &relPath) const;

    QSharedPointer<WConfigDirRef>
    subDir(const QString &name) const; // reference to a subdirectory

private:
    WConfigViewer *m_viewer;
    bool validateRelPath(const QString &relPath) const;
};

} // namespace we::config

#endif // WCONFIGREF_H