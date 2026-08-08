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
#ifndef WCONFIGWIDGET_H
#define WCONFIGWIDGET_H

#include <QDialog>
#include <QTreeWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include "WConfig.h"
#include "WConfigItemWidget.h"

namespace we::config {

class WE_EXPORT WConfigWidget : public QDialog
{
    Q_OBJECT
public:
    explicit WConfigWidget(WConfig* config, QWidget* parent = nullptr);
    ~WConfigWidget();
    void addVerification(std::function<bool(WConfig*)> func);

private slots:
    void onTreeItemClicked(QTreeWidgetItem* item, int column);
    void onSaveClicked();
    void onCancelClicked();
    void onAddItemClicked();
    void onRemoveItemClicked();

private:
    WConfig* m_config;
    QTreeWidget* m_treeWidget;
    QScrollArea* m_scrollArea;
    QTreeWidgetItem *m_rootItem = nullptr;
    QWidget* m_contentWidget;
    QVBoxLayout* m_contentLayout;
    QLabel* m_restartLabel;
    QPushButton* m_cancelButton;
    QPushButton* m_saveButton;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;

    WConfigViewer* m_currentViewer = nullptr;
    WConfigItemWidget* m_selectedItemWidget = nullptr;
    QList<WConfigItemWidget*> m_itemWidgets;
    QList<std::function<bool(WConfig*)>> m_verifications;
    void syncCurrentPage();
    void buildTree(WConfigViewer* viewer, QTreeWidgetItem* parentItem = nullptr);
    void displayConfig(WConfigViewer* viewer);
    void refreshCurrentPage();
    void setupAcceptPolicyButtons(WConfigViewer* viewer);
    void updateRestartPrompt();
    void forEachConfigData(std::function<void(WConfigDataBase*)> func);
};

} // namespace we::config

#endif // WCONFIGWIDGET_H