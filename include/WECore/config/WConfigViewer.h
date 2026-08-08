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
#ifndef WCONFIGVIEWER_H
#define WCONFIGVIEWER_H

#include "WConfigDataBase.h"
#include <QAtomicInt>
#include <QList>
#include <QString>

namespace we::config {

enum class AcceptPolicy { DEFAULT, ACCEPT, REFUSE };

// 策略传播模式
enum class PolicyPropagation {
    ApplyToSelfOnly, // 仅当前目录
    ApplyRecursively // 递归应用到所有子目录
};

// 覆盖模式
enum class OverwriteMode {
    KeepNonDefault, // 子目录若非 DEFAULT 则保留，否则覆盖
    ForceOverwrite  // 强制覆盖所有子目录（包括非 DEFAULT）
};

class WE_EXPORT WConfigViewer {
public:
    WConfigViewer(const QString &name, WConfigViewer *parent = nullptr);
    ~WConfigViewer();

    QString name() const { return m_name; }
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

    AcceptPolicy effectiveAcceptPolicy() const;
    DeletionPolicy deletionPolicy() const { return m_deletionPolicy; }
    void setDeletionPolicy(DeletionPolicy policy) { m_deletionPolicy = policy; }

    bool isFromTemplate() const { return m_isFromTemplate; }
    void setIsFromTemplate(bool fromTemplate) { m_isFromTemplate = fromTemplate; }

    // 目录策略设置
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
    QString m_displayName; // 显示名称
    QString m_description; // 描述（用于Tooltip）
};

} // namespace we::config

#endif // WCONFIGVIEWER_H