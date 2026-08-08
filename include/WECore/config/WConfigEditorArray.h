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
#ifndef WCONFIGEDITORARRAY_H
#define WCONFIGEDITORARRAY_H

#include "WConfigDataArray.h"
#include "WConfigEditorBase.h"
#include <QPushButton>
#include <QTreeWidget>

namespace we::config {

class WE_EXPORT WConfigEditorArray : public WConfigEditorBase {
public:
    WConfigEditorArray(WConfigItemWidget *parent = nullptr);
    void createEditor() override;
    void setConfigData(WConfigDataBase *data) override;
    WConfigDataArray *getData();
    WConfigDataBase *configData() override;

private slots:
    void onAddArrayElement();
    void onRemoveArrayElement();
    void onInsertBefore();
    void onInsertAfter();
    void onArrayItemChanged(QTreeWidgetItem *item, int column);
    void insertElementAt(int position);

private:
    void refreshTree();
    QTreeWidget *m_arrayTree = nullptr;
    QPushButton *m_addArrayBtn = nullptr;
    QPushButton *m_removeArrayBtn = nullptr;
    QPushButton *m_insertAfterBtn = nullptr;
    QPushButton *m_insertBeforeBtn = nullptr;
};

} // namespace we::config

#endif // WCONFIGEDITORARRAY_H