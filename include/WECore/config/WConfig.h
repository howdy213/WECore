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
#ifndef WCONFIG_H
#define WCONFIG_H

#include "WConfigDocument.h"
#include "WECore/def/wedef.h"
#include <QObject>
#include <QReadWriteLock>
#include <QSettings>

namespace we::config {

class WE_EXPORT WConfig : public QObject {
    Q_OBJECT
public:
    explicit WConfig(QObject *parent = nullptr);
    ~WConfig() override;

    bool initialize(const QString &configFile,
                    WConfigTemplate *configTemplate = nullptr);
    bool initialize(QSettings *settings,
                    WConfigTemplate *configTemplate = nullptr);
    WConfigDocument *document() const { return m_document; }

    QVariant getValueDirect(const QString &path) const;
    bool setValueDirect(const QString &path, const QVariant &value,
                        bool force = false);
    QVariant getTemporaryValue(const QString &path) const;
    bool setTemporaryValue(const QString &path, const QVariant &value);
    QVariant getValue(const QString &path) const;
    bool setValue(const QString &path, const QVariant &value, bool force = false);

    WConfigItemInfo getInfo(const QString &path) const;
    bool hasProperty(const QString &path, Property prop) const;

    QReadWriteLock *lock() const { return &m_lock; }
    QSharedPointer<WConfigItemRef> createItemRef(const QString &path);
    QSharedPointer<WConfigDirRef> createDirRef(const QString &path);

    bool save();                          // 支持存储在QSettings中
    bool saveAs(const QString &filePath); // 只支持文件
    QStringList lastSaveErrors() const;
    void resetToDefaults();

    bool hasRestartRequiredChanges() const;
    void setHasRestartRequiredChanges(bool value);

    operator QReadWriteLock *() const { return &m_lock; }
signals:
    void configChanged(we::config::WConfigDataBase *data);

private:
    QSettings *m_settings = nullptr; // 只用于以传入setting的初始化的读取/保存
    WConfigDocument *m_document = nullptr;
    QString m_currentFilePath;
    bool m_hasRestartRequiredChanges;
    QStringList m_lastSaveErrors;
    mutable QReadWriteLock m_lock;
};

} // namespace we::config

#endif // WCONFIG_H