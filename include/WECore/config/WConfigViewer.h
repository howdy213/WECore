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
#ifndef WCONFIGVIEWER_H
#define WCONFIGVIEWER_H

#include "WConfigDataBase.h"
#include <QAtomicInt>
#include <QList>
#include <QPair>
#include <QString>
#include <functional>

class QLayout;

namespace we::config {

class WConfigGroupWidget;

// Render mode of a single member item inside a merged config group
enum class WConfigItemMode {
    Automatic,  // resolved from data->type()
    Inline,     // title on the left, on the same row as the editor (for scalar items)
    Full,       // full widget (for Array/Object)
    ButtonOnly, // button only (for Action)
    Hidden      // hidden
};

// Group-level layout builder, invoked once all member widgets exist; lets a group
// lay out its members freely via group->memberWidget(key). The returned layout is
// taken over by the group and installed on its member container; nullptr keeps the
// default vertical layout.
using WConfigGroupLayoutBuilder =
    std::function<QLayout *(WConfigGroupWidget *group)>;

// Viewer-level merged config group spec
struct WE_EXPORT WConfigGroupSpec {
    QString groupId;
    QString displayName;                 // shared title; empty means no title, layout container only
    QString description;                 // written into the tooltip
    QList<QPair<QString, WConfigItemMode>> members; // { itemKey, mode }
    // Builds the member-area layout at creation time (full group-level custom layout).
    WConfigGroupLayoutBuilder layoutBuilder;
};

enum class AcceptPolicy { DEFAULT, ACCEPT, REFUSE };

// How a policy is propagated
enum class PolicyPropagation {
    ApplyToSelfOnly, // current directory only
    ApplyRecursively // to all subdirectories as well
};

// Overwrite behaviour when applying a policy
enum class OverwriteMode {
    KeepNonDefault, // keep a subdirectory policy unless it is DEFAULT, otherwise overwrite
    ForceOverwrite  // overwrite all subdirectories, including non-DEFAULT ones
};

// Configuration tree node: acts both as a directory (holding subdirectories and
// items) and as a holder of items directly. Also carries access/deletion policy,
// the lock count and merged config groups (WConfigGroupSpec).
class WE_EXPORT WConfigViewer {
public:
    WConfigViewer(const QString &name, WConfigViewer *parent = nullptr);
    ~WConfigViewer();

    QString name() const { return m_name; }
    /// Rename this directory. Only used when a sub-config is mounted: its own root
    /// is unnamed and must take the name of the mount point in the host tree.
    void setName(const QString &name) { m_name = name; }
    /// Reparent this directory. Only used for mounted sub-config roots: their own
    /// root starts orphaned and must point at the host directory they live in,
    /// otherwise fullPath(), policy and lock lookup would not see the host tree.
    void setParent(WConfigViewer *parent) { m_parent = parent; }
    QString fullPath() const;
    WConfigViewer *parent() const { return m_parent; }

    WConfigViewer *findChildViewer(const QString &path);
    WConfigDataBase *findConfigData(const QString &path);

    bool addChild(WConfigViewer *child, bool force = false);
    bool removeChild(WConfigViewer *child, bool force = false);
    bool addConfigData(WConfigDataBase *data, bool force = false);
    bool removeConfigData(WConfigDataBase *data, bool force = false);
    bool clearAll(bool force = false);

    QList<WConfigViewer *> &mutableChildren() { return m_children; }
    QList<WConfigDataBase *> &mutableConfigData() { return m_configData; }

    WConfigDataBase *getConfigData(const QString &key) const;
    WConfigViewer *findOrCreateChild(const QString &name);

    const QList<WConfigViewer *> &children() const { return m_children; }
    const QList<WConfigDataBase *> &allConfigData() const { return m_configData; }
    QString displayName() const {
        return m_displayName.isEmpty() ? m_name : m_displayName;
    }
    void setDisplayName(const QString &name) { m_displayName = name; }
    QString description() const { return m_description; }
    void setDescription(const QString &desc) { m_description = desc; }
    AcceptPolicy acceptPolicy() const { return m_acceptPolicy; }

    // ---- Merged config groups ----
    const QList<WConfigGroupSpec *> &groups() const { return m_groups; }
    WConfigGroupSpec *addGroup(const WConfigGroupSpec &spec); // copies the spec, returns a stable pointer it owns
    bool removeGroup(const QString &groupId);
    WConfigGroupSpec *groupById(const QString &groupId) const;
    void clearGroups();
    void setGroups(const QList<WConfigGroupSpec *> &in); // replaces all groups and takes ownership of the pointers (reserved for plugin hooks)

    AcceptPolicy effectiveAcceptPolicy() const;
    DeletionPolicy deletionPolicy() const { return m_deletionPolicy; }
    void setDeletionPolicy(DeletionPolicy policy) { m_deletionPolicy = policy; }

    bool isFromTemplate() const { return m_isFromTemplate; }
    void setIsFromTemplate(bool fromTemplate) { m_isFromTemplate = fromTemplate; }

    // Directory policy (createIfNotExist is reserved; the current implementation ignores it)
    void setDirectoryPolicy(
        AcceptPolicy policy,
        PolicyPropagation propagation = PolicyPropagation::ApplyToSelfOnly,
        OverwriteMode overwrite = OverwriteMode::KeepNonDefault,
        bool createIfNotExist = true);

public:
    void lock() { ++m_lockCount; }
    void unlock() {
        if (m_lockCount > 0)
            --m_lockCount;
    }
    bool isLocked() const { return m_lockCount > 0; }
    bool isEffectivelyLocked() const;

private:
    QAtomicInt m_lockCount = 0;
    QString m_name;
    WConfigViewer *m_parent;
    QList<WConfigViewer *> m_children;
    QList<WConfigDataBase *> m_configData;
    AcceptPolicy m_acceptPolicy = AcceptPolicy::DEFAULT;
    DeletionPolicy m_deletionPolicy = DeletionPolicy::AllowNonTemplateOnly;
    bool m_isFromTemplate = false;
    QString m_displayName;
    QString m_description; // shown as a tooltip
    QList<WConfigGroupSpec *> m_groups; // merged config groups (owned and freed by this class)
};

} // namespace we::config

#endif // WCONFIGVIEWER_H