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
#include "WECore/config/WConfigEditorObject.h"
#include "WECore/config/WConfigDataDef.h"
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>
#include <memory>

namespace we::config {

int WConfigEditorObject::s_maxDepth = 2;

WConfigEditorObject::WConfigEditorObject(WConfigItemWidget *parent)
    : WConfigEditorBase(parent) {
    m_type = DataType::Object;
}

void WConfigEditorObject::createEditor() {
    m_objectTree = new QTreeWidget(this);
    m_objectTree->setHeaderLabels({tr("Key"), tr("Value")});
    m_objectTree->header()->setSectionResizeMode(0,
                                                 QHeaderView::ResizeToContents);
    m_objectTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_objectTree->setEditTriggers(QAbstractItemView::DoubleClicked |
                                  QAbstractItemView::SelectedClicked);
    connect(m_objectTree, &QTreeWidget::itemChanged, this,
            &WConfigEditorObject::onObjectItemChanged);
    connect(m_objectTree, &QTreeWidget::currentItemChanged, this,
            [this](QTreeWidgetItem *current, QTreeWidgetItem *) {
        m_selectedItem = current;
    });

    m_objectTree->setItemDelegateForColumn(0, new NoEditColumnDelegate(this));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_objectTree);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_addButton = new QPushButton(tr("Add Child"), this);
    m_removeButton = new QPushButton(tr("Remove Selected"), this);
    btnLayout->addWidget(m_addButton);
    btnLayout->addWidget(m_removeButton);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    connect(m_addButton, &QPushButton::clicked, this,
            &WConfigEditorObject::onAddChildItem);
    connect(m_removeButton, &QPushButton::clicked, this,
            &WConfigEditorObject::onRemoveChildItem);

    setLayout(mainLayout);
    connect(m_objectTree, &QTreeWidget::itemChanged, this,
            [this]() { emit valueChanged(); });
}

void WConfigEditorObject::setConfigData(WConfigDataBase *data) {
    m_data = data;
    m_objData = static_cast<WConfigDataObject *>(data);
    if (!m_objData)
        return;
    m_objectTree->clear();
    // 传入 m_objData 作为顶层父对象，深度为0
    loadObjectChildren(m_objData->content(), nullptr, 0, m_objData);
    setupButtonsForMode();
}

void WConfigEditorObject::syncTreeToData() {
    if (!m_objData)
        return;
    for (int i = 0; i < m_objectTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_objectTree->topLevelItem(i);
        syncItemToData(item, m_objData->content());
    }
}

void WConfigEditorObject::syncItemToData(QTreeWidgetItem *item,
                                         WConfigViewer *viewer) {
    if (!item || !viewer)
        return;
    WConfigDataBase *data =
        item->data(0, Qt::UserRole).value<WConfigDataBase *>();
    if (!data)
        return;
    if (data->type() == DataType::Object) {
        // 对象类型：递归处理其子项（如果有子项）
        for (int i = 0; i < item->childCount(); ++i) {
            syncItemToData(item->child(i),
                           static_cast<WConfigDataObject *>(data)->content());
        }
    } else {
        // 基本类型：从 EditRole 读取值并设置
        QVariant val = item->data(1, Qt::EditRole);
        if (val.isValid()) {
            data->setTemporary(val);
        }
    }
}

void WConfigEditorObject::setupButtonsForMode() {
    if (!m_objData)
        return;
    ObjectEditMode mode = m_objData->editMode();
    bool canModify = (mode == ObjectEditMode::ModifyOnly ||
                      mode == ObjectEditMode::FullControl);
    bool canAddRemove = (mode == ObjectEditMode::FullControl);

    m_objectTree->setEditTriggers(canModify ? (QAbstractItemView::DoubleClicked |
                                               QAbstractItemView::SelectedClicked)
                                            : QAbstractItemView::NoEditTriggers);
    m_addButton->setVisible(canAddRemove);
    m_removeButton->setVisible(canAddRemove);

    if (mode == ObjectEditMode::ReadOnly) {
        m_objectTree->setEnabled(false);
    } else {
        m_objectTree->setEnabled(true);
    }
}

WConfigDataObject *WConfigEditorObject::getData() {
    if (!m_objData)
        return nullptr;
    syncTreeToData();
    return m_objData;
}

WConfigDataBase *WConfigEditorObject::configData() { return getData(); }

// 加载对象子项
void WConfigEditorObject::loadObjectChildren(WConfigViewer *viewer,
                                             QTreeWidgetItem *parentItem,
                                             int depth,
                                             WConfigDataObject *parentObj) {
    if (!viewer || depth > s_maxDepth)
        return;
    for (WConfigDataBase *childData : viewer->allConfigData()) {
        if (!childData)
            continue;
        QTreeWidgetItem *item = parentItem ? new QTreeWidgetItem(parentItem)
                                           : new QTreeWidgetItem(m_objectTree);
        item->setData(0, Qt::DisplayRole, childData->key());
        item->setData(0, Qt::UserRole, QVariant::fromValue(childData));
        // 存储父对象指针 (UserRole+1)
        item->setData(0, Qt::UserRole + 1, QVariant::fromValue(parentObj));
        // 存储深度 (UserRole+2)
        item->setData(0, Qt::UserRole + 2, depth + 1); // 子项深度为当前深度+1

        // 根据类型设置值和编辑角色
        if (childData->type() == DataType::Object) {
            // 对象类型：显示占位符，不提供 EditRole，禁止编辑
            item->setData(1, Qt::DisplayRole, "{...}");
            item->setData(1, Qt::EditRole, QVariant()); // 清空
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        } else {
            // 基本类型：显示值，并设置 EditRole 以支持类型感知编辑
            QVariant val = childData->getTemporary();
            item->setData(1, Qt::EditRole, val);
            bool editable =
                (m_objData && m_objData->editMode() != ObjectEditMode::ReadOnly);
            if (editable)
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            else
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }

        // 递归加载子对象
        if (childData->type() == DataType::Object && depth < s_maxDepth) {
            auto *childObj = static_cast<WConfigDataObject *>(childData);
            // 子对象的父对象就是 childObj，深度+1
            loadObjectChildren(childObj->content(), item, depth + 1, childObj);
        }
    }
}

void WConfigEditorObject::onObjectItemChanged(QTreeWidgetItem *item,
                                              int column) {
    if (column == 0) {
        // 键名不允许修改，直接还原
        WConfigDataBase *childData =
            item->data(0, Qt::UserRole).value<WConfigDataBase *>();
        if (childData) {
            item->setData(0, Qt::DisplayRole, childData->key());
        }
        return;
    }
    if (column != 1)
        return;
    WConfigDataBase *childData =
        item->data(0, Qt::UserRole).value<WConfigDataBase *>();
    if (!childData)
        return;
    // 禁止编辑对象类型的值列
    if (childData->type() == DataType::Object) {
        // 还原为占位符
        item->setData(1, Qt::DisplayRole, "{...}");
        return;
    }
    QVariant newValue = item->data(1, Qt::EditRole);
    childData->setTemporary(newValue);
}

static int getObjectDepth(WConfigDataObject *target, WConfigDataObject *root) {
    if (target == root)
        return 0;
    for (WConfigDataBase *child : root->content()->allConfigData()) {
        if (child->type() == DataType::Object) {
            WConfigDataObject *obj = static_cast<WConfigDataObject *>(child);
            if (obj == target)
                return 1;
            int subDepth = getObjectDepth(target, obj);
            if (subDepth != -1)
                return subDepth + 1;
        }
    }
    return -1;
}

void WConfigEditorObject::onAddChildItem() {
    // 验证对象数据
    if (!m_objData) {
        qWarning() << "WConfigEditorObject::onAddChildItem: m_objData is null";
        return;
    }

    // 检查编辑模式是否允许添加
    if (m_objData->editMode() != ObjectEditMode::FullControl) {
        QMessageBox::warning(this, tr("Not Allowed"),
                             tr("Cannot add child items in current edit mode "
                                "(requires FullControl)."));
        return;
    }

    // 确定父对象：如果选中项是 Object，则添加到其内部；否则添加到当前对象
    WConfigDataObject *parentObj = m_objData;
    WConfigDataBase *selectedData = nullptr;
    QTreeWidgetItem *selectedItem = m_selectedItem;
    if (selectedItem) {
        selectedData = selectedItem->data(0, Qt::UserRole).value<WConfigDataBase *>();
        if (selectedData && selectedData->type() == DataType::Object) {
            parentObj = static_cast<WConfigDataObject *>(selectedData);
        }
    }

    // 计算父对象的深度（用于深度限制）
    int parentDepth = getObjectDepth(parentObj, m_objData);
    if (parentDepth == -1) {
        // 理论上不可能，若发生则视为根深度
        parentDepth = 0;
    }

    // 弹出添加对话框
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Add Child Item"));
    QFormLayout *form = new QFormLayout(&dialog);

    QLineEdit *keyEdit = new QLineEdit(&dialog);
    form->addRow(tr("Key:"), keyEdit);

    QComboBox *typeCombo = new QComboBox(&dialog);
    typeCombo->addItems({"String", "Int", "Double", "Bool", "Object"});
    form->addRow(tr("Type:"), typeCombo);

    QLineEdit *valueEdit = new QLineEdit(&dialog);
    QLabel *valueLabel = new QLabel(tr("Value:"), &dialog);
    form->addRow(valueLabel, valueEdit);

    // 类型切换时隐藏/显示值输入框
    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int idx) {
                bool isObject = (typeCombo->itemText(idx) == "Object");
                valueEdit->setVisible(!isObject);
                valueLabel->setVisible(!isObject);
            });

    bool initialIsObject = (typeCombo->currentText() == "Object");
    valueEdit->setVisible(!initialIsObject);
    valueLabel->setVisible(!initialIsObject);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return;

    // 获取输入数据
    QString key = keyEdit->text().trimmed();
    if (key.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Key cannot be empty."));
        return;
    }
    if (parentObj->findChildData(key)) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Child with key '%1' already exists.").arg(key));
        return;
    }

    QString typeStr = typeCombo->currentText();

    // 检查深度限制
    if (typeStr == "Object" && parentDepth >= s_maxDepth) {
        QMessageBox::warning(
            this, tr("Depth Limit Exceeded"),
            tr("Cannot add nested object: maximum depth (%1) reached.")
                .arg(s_maxDepth));
        return;
    }

    // 创建新数据项
    std::unique_ptr<WConfigDataBase> newDataPtr;
    if (typeStr == "Object") {
        WConfigDataObject *obj = new WConfigDataObject();
        WConfigItemInfo info;
        info.displayName(key).description("");
        obj->init(key, info);
        obj->setEditMode(parentObj->editMode());
        obj->setDeletionPolicy(parentObj->deletionPolicy());
        obj->setIsFromTemplate(parentObj->isFromTemplate());
        newDataPtr.reset(obj);
    } else {
        QVariant value;
        bool ok = true;
        if (typeStr == "String") {
            value = valueEdit->text();
        } else if (typeStr == "Int") {
            int intVal = valueEdit->text().toInt(&ok);
            if (!ok) {
                QMessageBox::warning(this, tr("Error"), tr("Invalid integer."));
                return;
            }
            value = intVal;
        } else if (typeStr == "Double") {
            double doubleVal = valueEdit->text().toDouble(&ok);
            if (!ok) {
                QMessageBox::warning(this, tr("Error"), tr("Invalid double."));
                return;
            }
            value = doubleVal;
        } else if (typeStr == "Bool") {
            QString text = valueEdit->text().trimmed().toLower();
            if (text == "true" || text == "1" || text == "yes")
                value = true;
            else if (text == "false" || text == "0" || text == "no")
                value = false;
            else {
                QMessageBox::warning(this, tr("Error"),
                                     tr("Invalid boolean (true/false)."));
                return;
            }
        }
        WConfigItemInfo info;
        info.displayName(key).defaultValue(value);
        if (typeStr == "String")
            newDataPtr.reset((new WConfigDataString)->init(key, info));
        else if (typeStr == "Int")
            newDataPtr.reset((new WConfigDataInt)->init(key, info));
        else if (typeStr == "Double")
            newDataPtr.reset((new WConfigDataDouble)->init(key, info));
        else if (typeStr == "Bool")
            newDataPtr.reset((new WConfigDataBool)->init(key, info));
    }

    if (!newDataPtr) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to create item."));
        return;
    }

    // 添加新项到父对象（优先插入到选中项之后）
    bool inserted = false;
    if (selectedData && selectedData->type() != DataType::Object) {
        WConfigViewer *content = parentObj->content();
        if (content) {
            if (content->getConfigData(key) || content->findChildViewer(key)) {
                QMessageBox::warning(
                    this, tr("Error"),
                    tr("An item with key '%1' already exists.").arg(key));
                return;
            }
            QList<WConfigDataBase *> &list = content->mutableConfigData();
            int index = list.indexOf(selectedData);
            if (index >= 0) {
                WConfigDataBase *rawData = newDataPtr.release();
                rawData->setParent(content);
                rawData->setIsFromTemplate(parentObj->isFromTemplate());
                list.insert(index + 1, rawData);
                inserted = true;
            }
        }
    }

    if (!inserted) {
        if (!parentObj->addChildData(newDataPtr.release(), parentObj->isFromTemplate())) {
            QMessageBox::warning(this, tr("Error"), tr("Failed to add child item."));
            return;
        }
    }

    // 刷新树显示并发射信号
    m_objectTree->clear();
    loadObjectChildren(m_objData->content(), nullptr, 0, m_objData);
    m_objectTree->expandAll();
    emit valueChanged();
}

void WConfigEditorObject::onRemoveChildItem() {
    if (!m_objData) {
        qWarning() << "WConfigEditorObject::onRemoveChildItem: m_objData is null";
        return;
    }
    if (m_objData->editMode() != ObjectEditMode::FullControl) {
        QMessageBox::warning(this, tr("Operation Not Allowed"),
                             tr("Current object edit mode does not allow deletion."));
        return;
    }

    if (!m_selectedItem) {
        QMessageBox::information(this, tr("No Selection"),
                                 tr("Please select an item to remove."));
        return;
    }

    WConfigDataBase *childData =
        m_selectedItem->data(0, Qt::UserRole).value<WConfigDataBase *>();
    if (!childData) {
        QMessageBox::warning(this, tr("Error"), tr("Invalid selected item."));
        return;
    }

    WConfigDataObject *parentObj =
        m_selectedItem->data(0, Qt::UserRole + 1).value<WConfigDataObject *>();
    if (!parentObj) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot determine parent object (data corruption)."));
        return;
    }

    // 检查父对象的删除策略
    DeletionPolicy policy = parentObj->deletionPolicy();
    if (policy == DeletionPolicy::DisallowAll) {
        QMessageBox::warning(this, tr("Deletion Forbidden"),
                             tr("Deletion is not allowed for items in this object."));
        return;
    }
    if (policy == DeletionPolicy::AllowNonTemplateOnly && childData->isFromTemplate()) {
        QMessageBox::warning(this, tr("Cannot Delete Template Item"),
                             tr("This item is defined by the template and cannot be deleted."));
        return;
    }

    QString key = childData->key();
    QString value = childData->getTemporary().toString();
    if (value.length() > 50)
        value = value.left(47) + "...";
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Confirm Deletion"),
        tr("Are you sure you want to delete the following child item?\n\n"
           "Key:   %1\n"
           "Value: %2")
            .arg(key, value),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;

    // 执行删除
    WConfigViewer *content = childData->parent();
    if (!content) {
        QMessageBox::warning(this, tr("Error"), tr("Item has no parent container."));
        return;
    }

    // 尝试从父容器中移除，检查返回值
    if (!content->removeConfigData(childData)) {
        QMessageBox::warning(this, tr("Deletion Failed"),
                             tr("Failed to remove the item from its parent container."));
        return;
    }

    delete childData;
    m_objectTree->clear();
    loadObjectChildren(m_objData->content(), nullptr, 0, m_objData);
    emit valueChanged();
    m_selectedItem = nullptr;
}

} // namespace we::config