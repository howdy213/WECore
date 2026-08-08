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
#ifndef WCONFIGEDITOROBJECT_H
#define WCONFIGEDITOROBJECT_H

#include "WConfigDataObject.h"
#include "WConfigEditorBase.h"
#include <QPushButton>
#include <QTreeWidget>

namespace we::config {

class WE_EXPORT WConfigEditorObject : public WConfigEditorBase {
    Q_OBJECT
public:
    WConfigEditorObject(WConfigItemWidget *parent = nullptr);
    void createEditor() override;
    void setConfigData(WConfigDataBase *data) override;
    void loadObjectChildren(WConfigViewer *viewer, QTreeWidgetItem *parentItem,
                            int depth, WConfigDataObject *parentObj);
    WConfigDataObject *getData();
    WConfigDataBase *configData() override;
    static int maxDepth() { return s_maxDepth; }
    static void setMaxDepth(int depth) { s_maxDepth = depth; }

private slots:
    void onObjectItemChanged(QTreeWidgetItem *item, int column);
    void onAddChildItem();
    void onRemoveChildItem();

private:
    void syncTreeToData();
    void syncItemToData(QTreeWidgetItem *item, WConfigViewer *viewer);
    void setupButtonsForMode();
    static int s_maxDepth;
    QTreeWidget *m_objectTree = nullptr;
    QPushButton *m_addButton = nullptr;
    QPushButton *m_removeButton = nullptr;
    WConfigDataObject *m_objData = nullptr;
    QTreeWidgetItem *m_selectedItem = nullptr; // 当前选中的项
};

} // namespace we::config

#endif // WCONFIGEDITOROBJECT_H