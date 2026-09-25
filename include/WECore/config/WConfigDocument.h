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
#ifndef WCONFIGDOCUMENT_H
#define WCONFIGDOCUMENT_H

#include "WConfigTemplate.h"
#include "WConfigViewer.h"
#include <QObject>
#include <QString>
#include <QVariant>

namespace we::config {

class WConfigFileStorage;
class WConfigSettingsStorage;

/**
 * @brief In-memory configuration tree container.
 *
 * Only responsible for building and serializing config items/subdirectories:
 * - setTemplate() fills the default structure from a template;
 * - loadFromVariant() merges external data into the config tree;
 * - toVariant() exports the config tree as a nested QVariantMap.
 *
 * Actual reading/writing against a file or QSettings is handled by the
 * WConfigStorage implementations, which access the private
 * loadFromVariant()/saveToVariant() here directly through friend declarations.
 */
class WE_EXPORT WConfigDocument : public QObject {
    Q_OBJECT
    friend class WConfigFileStorage;
    friend class WConfigSettingsStorage;

public:
    explicit WConfigDocument(QObject *parent = nullptr);
    ~WConfigDocument() override;

    WConfigViewer *root() const { return m_root; }
    void setTemplate(WConfigTemplate *configTemplate);
    WConfigTemplate *configTemplate() const { return m_template; }

    /// Export the whole config tree as a nested map (keys are item/directory names).
    QVariant toVariant() const;
    /// Delete all data items and subdirectories under viewer; locked nodes refuse deletion when force=false.
    bool clearViewer(WConfigViewer *viewer, bool force = false);
    /// Write temporary values back to persistent values, making the current in-memory state the new persistent baseline.
    void syncToAllPersistent();

    /// Controls whether items not declared in the template may be created during load; defaults to false.
    void setAllowCreateOnLoad(bool allow) { m_allowCreateOnLoad = allow; }

private:
    void loadFromVariant(WConfigViewer *viewer, const QVariant &variant);
    QVariant saveToVariant(WConfigViewer *viewer) const;

private:
    bool m_allowCreateOnLoad = false;
    WConfigViewer *m_root;
    WConfigTemplate *m_template;
};

} // namespace we::config

#endif // WCONFIGDOCUMENT_H