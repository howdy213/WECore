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
#ifndef WCONFIGDOCUMENT_H
#define WCONFIGDOCUMENT_H

#include "WConfigTemplate.h"
#include "WConfigViewer.h"
#include <QObject>
#include <QSettings>
#include <QString>

namespace we::config {

class WE_EXPORT WConfigDocument : public QObject {
    Q_OBJECT
public:
    enum Format { Json, Ini, Unknown };

    explicit WConfigDocument(QObject *parent = nullptr);
    ~WConfigDocument() override;

    WConfigViewer *root() const { return m_root; }
    void setTemplate(WConfigTemplate *configTemplate);
    WConfigTemplate *configTemplate() const { return m_template; }

    bool load(const QString &filePath);
    bool save(const QString &filePath, QStringList &errors);

    static Format formatFromFilePath(const QString &filePath);
    QVariant toVariant() const;
    bool clearViewer(WConfigViewer *viewer, bool force = false);
    void syncToAllPersistent();

    void setAllowCreateOnLoad(bool allow) { m_allowCreateOnLoad = allow; }
    bool loadFromSettings(QSettings* settings);
    bool saveToSettings(QSettings* settings);
private:
    bool loadJson(const QString &filePath);
    bool loadIni(const QString &filePath);
    bool saveJson(const QString &filePath);
    bool saveIni(const QString &filePath);

    void loadFromVariant(WConfigViewer *viewer, const QVariant &variant);
    QVariant saveToVariant(WConfigViewer *viewer) const;

private:
    bool m_allowCreateOnLoad = false;   // 默认为 false
    WConfigViewer *m_root;
    WConfigTemplate *m_template;
};

} // namespace we::config

#endif // WCONFIGDOCUMENT_H