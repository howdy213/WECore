/**
 * @file wpluginconfigwidget.cpp
 * @brief Implementation of the plugin configuration tree editor.
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
#include "WECore/plugin/wpluginconfigwidget.h"

#include "WECore/def/wedef.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>

namespace we {

using namespace Consts;

WPluginConfigWidget::WPluginConfigWidget(WPluginConfigManager *manager,
                                         QWidget *parent)
    : QWidget(parent), manager_(*manager) {
    setupUi();
}

void WPluginConfigWidget::setupUi() {
    tree_ = new QTreeWidget(this);
    tree_->setColumnCount(2);
    tree_->setHeaderLabels({tr("KeyName"), tr("Path")});
    tree_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tree_->setAlternatingRowColors(true);
    tree_->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tree_->header()->setSectionResizeMode(1, QHeaderView::Stretch);

    detail_ = new QPlainTextEdit(this);
    detail_->setReadOnly(true);
    detail_->setPlaceholderText(tr("No configuration content."));

    addDirBtn_ = new QPushButton(tr("New Sub-directory"), this);
    addLeafBtn_ = new QPushButton(tr("New Leaf"), this);
    deleteBtn_ = new QPushButton(tr("Delete Node"), this);
    editPathBtn_ = new QPushButton(tr("Edit Path"), this);
    editFileBtn_ = new QPushButton(tr("Edit File"), this);
    refreshBtn_ = new QPushButton(tr("Refresh"), this);
    cancelEditBtn_ = new QPushButton(tr("Cancel"), this);
    cancelEditBtn_->setVisible(false);

    auto *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(addDirBtn_);
    btnLayout->addWidget(addLeafBtn_);
    btnLayout->addWidget(deleteBtn_);
    btnLayout->addWidget(editPathBtn_);
    btnLayout->addWidget(editFileBtn_);
    btnLayout->addWidget(refreshBtn_);
    btnLayout->addWidget(cancelEditBtn_);
    btnLayout->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tree_, 3);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(detail_, 2);

    connect(addDirBtn_, &QPushButton::clicked, this,
            &WPluginConfigWidget::onAddDirectory);
    connect(addLeafBtn_, &QPushButton::clicked, this,
            &WPluginConfigWidget::onAddLeaf);
    connect(deleteBtn_, &QPushButton::clicked, this,
            &WPluginConfigWidget::onDeleteNode);
    connect(editPathBtn_, &QPushButton::clicked, this,
            &WPluginConfigWidget::onEditPath);
    connect(editFileBtn_, &QPushButton::clicked, this,
            &WPluginConfigWidget::onEditFile);
    connect(refreshBtn_, &QPushButton::clicked, this,
            &WPluginConfigWidget::onRefresh);
    connect(cancelEditBtn_, &QPushButton::clicked, this,
            &WPluginConfigWidget::onCancelEdit);
    connect(tree_, &QTreeWidget::currentItemChanged, this,
            &WPluginConfigWidget::onCurrentItemChanged);
}

void WPluginConfigWidget::setRootJsonPath(const QString &path) {
    if (manager_.loadFromFile(path)) {
        rebuildTreeFromManager();
        return;
    }

    const QMessageBox::StandardButton ret = QMessageBox::question(
        this, tr("Configuration File Error"),
        tr("Cannot load the root configuration file: %1\nRecreate it as an "
           "empty configuration?")
            .arg(path),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (ret != QMessageBox::Yes)
        return;

    const QFileInfo fileInfo(path);
    QDir().mkpath(fileInfo.absolutePath());

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |
                   QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot create the root configuration file: %1")
                                 .arg(path));
        return;
    }
    QJsonObject rootObj;
    rootObj[Plugins::Plugins] = QJsonObject();
    file.write(QJsonDocument(rootObj).toJson(QJsonDocument::Indented));
    file.close();

    if (manager_.loadFromFile(path)) {
        rebuildTreeFromManager();
    } else {
        QMessageBox::warning(
            this, tr("Error"),
            tr("The root configuration file still cannot be loaded after "
               "recreating it. Check its permissions or content."));
    }
}

QString WPluginConfigWidget::rootJsonPath() const {
    return manager_.rootJsonPath();
}

void WPluginConfigWidget::refreshTree() {
    if (manager_.refresh())
        rebuildTreeFromManager();
}

QString WPluginConfigWidget::filePathForNode(WPluginTreeNode *node) const {
    if (!node)
        return QString();
    if (node == manager_.rootNode())
        return manager_.rootJsonPath();
    return node->absolutePath;
}

void WPluginConfigWidget::setSelectionMode(bool enable) {
    selectionMode_ = enable;
    addDirBtn_->setEnabled(!enable);
    addLeafBtn_->setEnabled(!enable);
    deleteBtn_->setEnabled(!enable);
    editPathBtn_->setEnabled(!enable);
    editFileBtn_->setEnabled(!enable);
    refreshBtn_->setEnabled(!enable);
}

bool WPluginConfigWidget::isSelectionMode() const { return selectionMode_; }

WPluginTreeNode *WPluginConfigWidget::selectedNode() const {
    QTreeWidgetItem *item = tree_->currentItem();
    if (!item)
        return nullptr;
    return nodeFromItem(item);
}

WPluginTreeNode *WPluginConfigWidget::selectedParentNode() const {
    WPluginTreeNode *node = selectedNode();
    if (!node)
        return nullptr;
    if (node == manager_.rootNode())
        return nullptr; // The root itself may act as a parent.
    if (node->isContainer)
        return node;
    return node->parent == manager_.rootNode() ? nullptr : node->parent;
}

void WPluginConfigWidget::rebuildTreeFromManager() {
    tree_->clear();

    // The root item stands for the root configuration file itself.
    auto *rootItem = new QTreeWidgetItem();
    rootItem->setText(0, tr("Root"));
    rootItem->setText(1, manager_.rootJsonPath());
    rootItem->setData(
        0, Qt::UserRole,
        QVariant::fromValue(reinterpret_cast<quintptr>(manager_.rootNode())));
    tree_->addTopLevelItem(rootItem);

    for (WPluginTreeNode *node : manager_.topLevelNodes())
        createItemForNode(node, rootItem);
    tree_->expandAll();
}

QTreeWidgetItem *
WPluginConfigWidget::createItemForNode(WPluginTreeNode *node,
                                       QTreeWidgetItem *parentItem) {
    auto *item = new QTreeWidgetItem();
    item->setText(0, node->key);
    item->setText(1, node->path);
    item->setData(0, Qt::UserRole,
                  QVariant::fromValue(reinterpret_cast<quintptr>(node)));

    if (parentItem)
        parentItem->addChild(item);
    else
        tree_->addTopLevelItem(item);

    for (WPluginTreeNode *child : std::as_const(node->children))
        createItemForNode(child, item);
    return item;
}

WPluginTreeNode *WPluginConfigWidget::nodeFromItem(QTreeWidgetItem *item) const {
    if (!item)
        return nullptr;
    return reinterpret_cast<WPluginTreeNode *>(
        item->data(0, Qt::UserRole).value<quintptr>());
}

void WPluginConfigWidget::updateDetailForNode(WPluginTreeNode *node) {
    detail_->setPlainText(manager_.readFileContent(node));
}

WPluginTreeNode *WPluginConfigWidget::parentForNewNode() const {
    QTreeWidgetItem *currentItem = tree_->currentItem();
    if (!currentItem)
        return nullptr;
    WPluginTreeNode *node = nodeFromItem(currentItem);
    if (!node || node == manager_.rootNode())
        return nullptr; // The root item adds to the root file.
    if (node->isContainer)
        return node;
    return (node->parent == manager_.rootNode()) ? nullptr : node->parent;
}

QString WPluginConfigWidget::absolutePathForNewNode(WPluginTreeNode *parent,
                                                    const QString &path) const {
    if (QDir::isAbsolutePath(path))
        return QDir::cleanPath(path);

    const QString baseDir = parent
                                ? QFileInfo(parent->absolutePath).absolutePath()
                                : QFileInfo(manager_.rootJsonPath())
                                      .absolutePath();
    return QDir(baseDir).filePath(path);
}

void WPluginConfigWidget::enterEditMode(WPluginTreeNode *node,
                                        const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const QMessageBox::StandardButton ret = QMessageBox::question(
            this, tr("File Not Found"),
            tr("File %1 does not exist. Create it?").arg(filePath),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (ret != QMessageBox::Yes)
            return;

        const QFileInfo fi(filePath);
        QDir().mkpath(fi.absolutePath());

        QFile createFile(filePath);
        if (!createFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Cannot create the file: %1").arg(filePath));
            return;
        }
        createFile.write("{}"); // Start from an empty JSON object.
        createFile.close();
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(
                this, tr("Error"),
                tr("Still cannot open the file after creating it: %1")
                    .arg(filePath));
            return;
        }
    }

    editing_ = true;
    editingNode_ = node;
    editingFilePath_ = filePath;

    detail_->setPlainText(QString::fromUtf8(file.readAll()));
    file.close();

    detail_->setReadOnly(false);
    editFileBtn_->setText(tr("Save"));

    addDirBtn_->setEnabled(false);
    addLeafBtn_->setEnabled(false);
    deleteBtn_->setEnabled(false);
    editPathBtn_->setEnabled(false);
    refreshBtn_->setEnabled(false);
    cancelEditBtn_->setVisible(true);
    tree_->setEnabled(false);
}

void WPluginConfigWidget::exitEditMode(bool save) {
    detail_->setReadOnly(true);
    detail_->clear();
    editing_ = false;
    editingNode_ = nullptr;
    editingFilePath_.clear();
    editFileBtn_->setText(tr("Edit File"));

    addDirBtn_->setEnabled(true);
    addLeafBtn_->setEnabled(true);
    deleteBtn_->setEnabled(true);
    editPathBtn_->setEnabled(true);
    refreshBtn_->setEnabled(true);
    cancelEditBtn_->setVisible(false);
    tree_->setEnabled(true);

    if (save) {
        refreshTree();
        return;
    }
    if (WPluginTreeNode *node = selectedNode())
        updateDetailForNode(node);
}

void WPluginConfigWidget::onAddDirectory() {
    WPluginTreeNode *parent = parentForNewNode();

    bool ok = false;
    const QString key =
        QInputDialog::getText(this, tr("New Sub-directory"), tr("KeyName:"),
                              QLineEdit::Normal, QString(), &ok);
    if (!ok || key.isEmpty())
        return;

    const QString path = QInputDialog::getText(
        this, tr("New Sub-directory"), tr("Path (relative or absolute):"),
        QLineEdit::Normal, QString(), &ok);
    if (!ok || path.isEmpty())
        return;

    const QString absPath = absolutePathForNewNode(parent, path);

    if (!QFileInfo::exists(absPath)) {
        const QFileInfo fileInfo(absPath);
        QDir().mkpath(fileInfo.absolutePath());

        QFile file(absPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |
                       QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Cannot create the file: %1").arg(absPath));
            return;
        }
        QJsonObject containerObj;
        containerObj[Plugins::Plugins] = QJsonObject();
        file.write(QJsonDocument(containerObj).toJson(QJsonDocument::Indented));
        file.close();
    }

    if (manager_.addNode(parent, key, path)) {
        rebuildTreeFromManager();
    } else {
        QMessageBox::warning(
            this, tr("Error"),
            tr("Failed to add the node. The KeyName may already exist, or the "
               "parent configuration file is not writable."));
    }
}

void WPluginConfigWidget::onAddLeaf() {
    WPluginTreeNode *parent = parentForNewNode();

    bool ok = false;
    const QString key =
        QInputDialog::getText(this, tr("New Leaf"), tr("KeyName:"),
                              QLineEdit::Normal, QString(), &ok);
    if (!ok || key.isEmpty())
        return;

    const QString path = QInputDialog::getText(
        this, tr("New Leaf"), tr("Path (relative or absolute):"),
        QLineEdit::Normal, QString(), &ok);
    if (!ok || path.isEmpty())
        return;

    const QString absPath = absolutePathForNewNode(parent, path);

    if (!QFileInfo::exists(absPath)) {
        const QFileInfo fileInfo(absPath);
        QDir().mkpath(fileInfo.absolutePath());

        QFile file(absPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |
                       QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Cannot create the file: %1").arg(absPath));
            return;
        }
        QJsonObject pluginObj;
        pluginObj[Plugin::Path] = ""; // Placeholder, editable later.
        file.write(QJsonDocument(pluginObj).toJson(QJsonDocument::Indented));
        file.close();
    }

    if (manager_.addNode(parent, key, path)) {
        rebuildTreeFromManager();
    } else {
        QMessageBox::warning(
            this, tr("Error"),
            tr("Failed to add the node. The KeyName may already exist, or the "
               "parent configuration file is not writable."));
    }
}

void WPluginConfigWidget::onDeleteNode() {
    WPluginTreeNode *node = selectedNode();
    if (!node || node == manager_.rootNode()) {
        QMessageBox::information(this, tr("Notice"),
                                 tr("Select a node to delete first."));
        return;
    }

    const QString msg =
        node->isContainer
            ? tr("Node '%1' is a container node; its child nodes will no "
                 "longer be listed (the file itself is not deleted).\n"
                 "Delete it?")
                  .arg(node->key)
            : tr("Delete leaf node '%1'?").arg(node->key);

    if (QMessageBox::question(this, tr("Confirm Deletion"), msg) !=
        QMessageBox::Yes)
        return;

    if (manager_.removeNode(node)) {
        rebuildTreeFromManager();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Failed to delete."));
    }
}

void WPluginConfigWidget::onEditPath() {
    WPluginTreeNode *node = selectedNode();
    if (!node || node == manager_.rootNode()) {
        QMessageBox::information(this, tr("Notice"),
                                 tr("Select a node to modify first."));
        return;
    }

    bool ok = false;
    const QString newPath = QInputDialog::getText(
        this, tr("Edit Path"), tr("Path:"), QLineEdit::Normal, node->path, &ok);
    if (!ok || newPath.isEmpty())
        return;

    if (manager_.setNodePath(node, newPath)) {
        rebuildTreeFromManager();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Failed to update the path."));
    }
}

void WPluginConfigWidget::onEditFile() {
    if (!editing_) {
        WPluginTreeNode *node = selectedNode();
        if (!node) {
            QMessageBox::information(this, tr("Notice"),
                                     tr("Select a node to edit first."));
            return;
        }
        const QString filePath = filePathForNode(node);
        if (filePath.isEmpty()) {
            QMessageBox::warning(this, tr("Error"),
                                 tr("The node has no associated file path."));
            return;
        }
        enterEditMode(node, filePath);
        return;
    }

    // Already editing: this click saves the content back to disk.
    if (!editingNode_)
        return;
    const QString newContent = detail_->toPlainText();
    QFile file(editingFilePath_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |
                   QIODevice::Text)) {
        QMessageBox::warning(
            this, tr("Error"),
            tr("Cannot write the file: %1").arg(editingFilePath_));
        return;
    }
    file.write(newContent.toUtf8());
    file.close();
    exitEditMode(true);
}

void WPluginConfigWidget::onCancelEdit() {
    if (editing_)
        exitEditMode(false);
}

void WPluginConfigWidget::onRefresh() { refreshTree(); }

void WPluginConfigWidget::onCurrentItemChanged(QTreeWidgetItem *current,
                                               QTreeWidgetItem *previous) {
    Q_UNUSED(previous)
    if (!current)
        return;
    WPluginTreeNode *node = nodeFromItem(current);
    if (!node)
        return;

    emit nodeSelected(node);
    if (!selectionMode_ && !editing_)
        updateDetailForNode(node);
}

} // namespace we