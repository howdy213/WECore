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
#ifndef WCONFIGEDITORACTION_H
#define WCONFIGEDITORACTION_H

#include "WConfigEditorBase.h"
#include <QPushButton>

namespace we::config {

class WE_EXPORT WConfigEditorAction : public WConfigEditorBase
{
    Q_OBJECT
public:
    WConfigEditorAction(WConfigItemWidget* parent = nullptr);
    void createEditor() override;
    void setConfigData(WConfigDataBase* data) override;
    WConfigDataBase* configData() override;

private slots:
    void onButtonClicked();

private:
    QPushButton* m_button = nullptr;
    WConfigDataAction* m_actionData = nullptr;
};

} // namespace we::config

#endif // WCONFIGEDITORACTION_H