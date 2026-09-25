/**
 * @file wpluginconfigwidget.h
 * @brief Editor widget for the plugin configuration tree.
 *
 * Shows the tree built by WPluginConfigManager and lets the user create,
 * rename, delete and edit entries. In selection mode the editing controls are
 * disabled and the widget only reports the picked node through its signals.
 *
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
#ifndef WPLUGINCONFIGWIDGET_H
#define WPLUGINCONFIGWIDGET_H

#include <QWidget>

#include "WECore/plugin/wpluginconfigmanager.h"

class QPlainTextEdit;
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

namespace we {

/**
 * @brief Tree editor for the plugin configuration files.
 *
 * The manager is borrowed, not owned; it must outlive this widget.
 */
class WE_EXPORT WPluginConfigWidget : public QWidget {
    Q_OBJECT

public:
    explicit WPluginConfigWidget(WPluginConfigManager *manager,
                                 QWidget *parent = nullptr);

    /// Loads @p path into the manager, offering to recreate a broken file.
    void setRootJsonPath(const QString &path);
    QString rootJsonPath() const;
    /// Reloads from disk and rebuilds the tree.
    void refreshTree();
    /// File backing @p node; the root node maps to the root JSON file.
    QString filePathForNode(WPluginTreeNode *node) const;
    /// Disables the editing controls, leaving only node selection.
    void setSelectionMode(bool enable);
    bool isSelectionMode() const;

    WPluginTreeNode *selectedNode() const;
    WPluginTreeNode *selectedParentNode() const;

signals:
    void nodeSelected(WPluginTreeNode *node);
    void nodeDoubleClicked(WPluginTreeNode *node);
    void parentNodeChosen(WPluginTreeNode *node);

private slots:
    void onAddDirectory();
    void onAddLeaf();
    void onDeleteNode();
    void onEditPath();
    void onEditFile();
    void onCancelEdit();
    void onRefresh();
    void onCurrentItemChanged(QTreeWidgetItem *current,
                              QTreeWidgetItem *previous);

private:
    void setupUi();
    void rebuildTreeFromManager();
    QTreeWidgetItem *createItemForNode(WPluginTreeNode *node,
                                       QTreeWidgetItem *parentItem);
    WPluginTreeNode *nodeFromItem(QTreeWidgetItem *item) const;
    void updateDetailForNode(WPluginTreeNode *node);

    WPluginTreeNode *parentForNewNode() const;
    QString absolutePathForNewNode(WPluginTreeNode *parent,
                                   const QString &path) const;
    void enterEditMode(WPluginTreeNode *node, const QString &filePath);
    void exitEditMode(bool save);

    WPluginConfigManager &manager_;
    QTreeWidget *tree_ = nullptr;
    QPlainTextEdit *detail_ = nullptr;
    QPushButton *addDirBtn_ = nullptr;
    QPushButton *addLeafBtn_ = nullptr;
    QPushButton *deleteBtn_ = nullptr;
    QPushButton *editPathBtn_ = nullptr;
    QPushButton *editFileBtn_ = nullptr;
    QPushButton *refreshBtn_ = nullptr;
    QPushButton *cancelEditBtn_ = nullptr;
    bool selectionMode_ = false;

    bool editing_ = false;
    QString editingFilePath_;
    WPluginTreeNode *editingNode_ = nullptr;
};

} // namespace we

#endif // WPLUGINCONFIGWIDGET_H