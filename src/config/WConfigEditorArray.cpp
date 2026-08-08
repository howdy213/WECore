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
#include "WECore/config/WConfigEditorArray.h"
#include <QBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStyledItemDelegate>

namespace we::config {

WConfigEditorArray::WConfigEditorArray(WConfigItemWidget *parent)
    : WConfigEditorBase(parent) {
    m_type = DataType::Array;
}

void WConfigEditorArray::createEditor() {
    auto *arrayData = static_cast<WConfigDataArray *>(m_data);
    Q_UNUSED(arrayData);
    QWidget *container = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_arrayTree = new QTreeWidget(container);
    m_arrayTree->setHeaderLabels({tr("Index"), tr("Value")});
    m_arrayTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_arrayTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_arrayTree->setEditTriggers(QAbstractItemView::DoubleClicked |
                                 QAbstractItemView::SelectedClicked);
    mainLayout->addWidget(m_arrayTree);

    m_arrayTree->setItemDelegateForColumn(0, new NoEditColumnDelegate(this));

    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_addArrayBtn = new QPushButton(tr("Append Element"), container);
    m_insertBeforeBtn = new QPushButton(tr("Insert Before"), container);
    m_insertAfterBtn = new QPushButton(tr("Insert After"), container);
    m_removeArrayBtn = new QPushButton(tr("Remove Selected"), container);
    btnLayout->addWidget(m_addArrayBtn);
    btnLayout->addWidget(m_insertBeforeBtn);
    btnLayout->addWidget(m_insertAfterBtn);
    btnLayout->addWidget(m_removeArrayBtn);
    mainLayout->addLayout(btnLayout);
    this->setLayout(mainLayout);

    connect(m_addArrayBtn, &QPushButton::clicked, this,
            &WConfigEditorArray::onAddArrayElement);
    connect(m_insertBeforeBtn, &QPushButton::clicked, this,
            &WConfigEditorArray::onInsertBefore);
    connect(m_insertAfterBtn, &QPushButton::clicked, this,
            &WConfigEditorArray::onInsertAfter);
    connect(m_removeArrayBtn, &QPushButton::clicked, this,
            &WConfigEditorArray::onRemoveArrayElement);
    connect(m_arrayTree, &QTreeWidget::itemChanged, this,
            &WConfigEditorArray::onArrayItemChanged);

    connect(m_arrayTree, &QTreeWidget::itemChanged, this,
            [this]() { emit valueChanged(); });
}

void WConfigEditorArray::setConfigData(WConfigDataBase *data) {
    m_data = data;
    auto *arrayData = static_cast<WConfigDataArray *>(data);
    if (!arrayData)
        return;
    m_arrayTree->clear();
    for (int i = 0; i < arrayData->count(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        QVariant val = arrayData->elementAt(i);
        item->setData(0, Qt::DisplayRole, QString::number(i));
        item->setData(1, Qt::EditRole, val);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        m_arrayTree->addTopLevelItem(item);
    }
}

WConfigDataArray *WConfigEditorArray::getData() {
    auto *arrayData = static_cast<WConfigDataArray *>(m_data);
    QVariantList list;
    for (int i = 0; i < m_arrayTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_arrayTree->topLevelItem(i);
        QVariant raw = item->text(1);
        QVariant value = convertVariantToType(raw, arrayData->elementType());
        list.append(value);
    }
    arrayData->setTemporary(list);
    emit valueChanged();
    return arrayData;
}

WConfigDataBase *WConfigEditorArray::configData() { return getData(); }

void WConfigEditorArray::onAddArrayElement() {
    auto *arrayData = static_cast<WConfigDataArray *>(m_data);
    QVariant defaultValue = defaultVariantForType(arrayData->elementType());
    if (!defaultValue.isValid())
        return;
    if (arrayData->addElement(defaultValue)) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, QString::number(m_arrayTree->topLevelItemCount()));
        item->setData(1, Qt::EditRole, defaultValue);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        m_arrayTree->addTopLevelItem(item);
        emit valueChanged();
    }
}

void WConfigEditorArray::onInsertBefore() {
    insertElementAt(-1); // 特殊标记表示在选中之前
}

void WConfigEditorArray::onInsertAfter() {
    insertElementAt(-2); // 特殊标记表示在选中之后
}

void WConfigEditorArray::insertElementAt(int position) {
    auto *arrayData = static_cast<WConfigDataArray *>(m_data);
    if (!arrayData)
        return;

    QTreeWidgetItem *selected = m_arrayTree->currentItem();
    if (!selected) {
        QMessageBox::information(this, tr("No Selection"),
                                 tr("Please select an element first."));
        return;
    }
    int index = selected->text(0).toInt();
    if (position == -1) { /* insert before */
    } else if (position == -2) {
        index = index + 1;
    } else
        return;

    QVariant defaultValue = defaultVariantForType(arrayData->elementType());
    if (!defaultValue.isValid())
        return;

    if (arrayData->insertElement(index, defaultValue)) {
        refreshTree();
        QTreeWidgetItem *newItem = m_arrayTree->topLevelItem(index);
        if (newItem)
            m_arrayTree->setCurrentItem(newItem);
    }
    emit valueChanged();
}

void WConfigEditorArray::refreshTree() {
    auto *arrayData = static_cast<WConfigDataArray *>(m_data);
    if (!arrayData)
        return;
    m_arrayTree->clear();
    for (int i = 0; i < arrayData->count(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        QVariant val = arrayData->elementAt(i);
        item->setData(0, Qt::DisplayRole, QString::number(i));
        item->setData(1, Qt::EditRole, val);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        m_arrayTree->addTopLevelItem(item);
    }
    // 更新所有行的索引显示
    for (int i = 0; i < m_arrayTree->topLevelItemCount(); ++i) {
        m_arrayTree->topLevelItem(i)->setData(0, Qt::DisplayRole,
                                              QString::number(i));
    }
}

void WConfigEditorArray::onRemoveArrayElement() {
    auto *arrayData = static_cast<WConfigDataArray *>(m_data);
    QTreeWidgetItem *selected = m_arrayTree->currentItem();
    if (!selected)
        return;
    int index = selected->text(0).toInt();
    if (arrayData->removeElement(index)) {
        delete selected;
        for (int i = 0; i < m_arrayTree->topLevelItemCount(); ++i)
            m_arrayTree->topLevelItem(i)->setText(0, QString::number(i));
        emit valueChanged();
    }
}

void WConfigEditorArray::onArrayItemChanged(QTreeWidgetItem *item, int column) {
    if (column != 1)
        return;
    auto *arrayData = static_cast<WConfigDataArray *>(m_data);
    int index = item->text(0).toInt();
    QVariant value = item->data(1, Qt::EditRole);
    arrayData->setElement(index, value);
}

} // namespace we::config