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

// ---------- 配置项引用 ----------
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

// ---------- 目录引用 ----------
class WE_EXPORT WConfigDirRef {
public:
    explicit WConfigDirRef(WConfigViewer *viewer);
    ~WConfigDirRef();
    // 仅当前层数据项
    QVariantMap toMap() const;
    // 仅更新当前层已存在的数据项
    bool fromMap(const QVariantMap &map, bool force = false);
    // 支持多级路径，返回数据值或子树的Variant
    QVariant getRelative(const QString &relPath) const;
    // 支持多级路径，自动创建中间目录
    bool setRelative(const QString &relPath, const QVariant &val,
                     bool force = false);

    void lock();
    void unlock();
    bool isLocked() const;
    bool isEffectivelyLocked() const;
    QString path() const;
    // 递归获取整个子树（包括子目录）
    QVariant toVariant() const;
    // 递归恢复整个子树（可创建新项/目录）
    bool fromVariant(const QVariant &variant, bool force = false);
    // 当前目录下所有子项名称（数据项+子目录）
    QStringList childKeys() const;
    // 检查相对路径是否存在
    bool contains(const QString &relPath) const;
    // 获取相对路径对应的数据类型（若为目录返回Object）
    DataType typeOf(const QString &relPath) const;

    QSharedPointer<WConfigDirRef>
    subDir(const QString &name) const; // 获取子目录引用

private:
    WConfigViewer *m_viewer;
    bool validateRelPath(const QString &relPath) const;
};

} // namespace we::config

#endif // WCONFIGREF_H