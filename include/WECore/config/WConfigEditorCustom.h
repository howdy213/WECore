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
#ifndef WCONFIGEDITORCUSTOM_H
#define WCONFIGEDITORCUSTOM_H

#include "WConfigEditorBase.h"
#include "WConfigCustomType.h"

namespace we::config {

// Editor for Custom items: delegates rendering to the widget produced by the
// type's editorFactory, looked up in WConfigCustomTypeRegistry by typeName().
class WE_EXPORT WConfigEditorCustom : public WConfigEditorBase {
    Q_OBJECT
public:
    WConfigEditorCustom(WConfigItemWidget *parent = nullptr);
    void createEditor() override;                        // Lazy build; see setConfigData
    void setConfigData(WConfigDataBase *data) override;  // Builds the factory widget on first call
    WConfigDataBase *configData() override;
    void refreshFromData() override;                     // Pushes data into the factory widget

private slots:
    void onCustomValueEdited();

private:
    QWidget *m_customWidget = nullptr;
    WCustomEditorInterface *m_iface = nullptr;
    bool m_built = false;
    void buildFromType(const QString &typeName);
};

} // namespace we::config

#endif // WCONFIGEDITORCUSTOM_H