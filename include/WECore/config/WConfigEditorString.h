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
#ifndef WCONFIGEDITORSTRING_H
#define WCONFIGEDITORSTRING_H

#include "WConfigDataString.h"
#include "WConfigEditorBase.h"
#include <QLineEdit>

namespace we::config {

// Editor for String items: a single-line edit bound to the temporary value.
class WE_EXPORT WConfigEditorString : public WConfigEditorBase
{
public:
    WConfigEditorString(WConfigItemWidget* parent = nullptr);
    void createEditor() override;
    void setConfigData(WConfigDataBase* data) override;
    WConfigDataString* getData();
    WConfigDataBase* configData() override;
    void refreshFromData() override;

private:
    QLineEdit* m_stringEdit = nullptr;
};

} // namespace we::config

#endif // WCONFIGEDITORSTRING_H