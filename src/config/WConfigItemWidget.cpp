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
#include "WECore/config/WConfigItemWidget.h"
#include "WECore/config/WConfigEditorAction.h"
#include "WECore/config/WConfigEditorArray.h"
#include "WECore/config/WConfigEditorBool.h"
#include "WECore/config/WConfigEditorDouble.h"
#include "WECore/config/WConfigEditorInt.h"
#include "WECore/config/WConfigEditorObject.h"
#include "WECore/config/WConfigEditorSelect.h"
#include "WECore/config/WConfigEditorString.h"
#include "WECore/config/WConfigRef.h"
#include <QFont>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStyle>
#include <QVBoxLayout>

namespace we::config {

WConfigItemWidget::WConfigItemWidget(WConfigDataBase *data, QWidget *parent)
    : QWidget(parent), m_data(data) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(6);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 标题行：显示名 + 重置按钮
    QHBoxLayout *titleLayout = new QHBoxLayout();
    m_nameLabel = new QLabel(m_data->info().displayName().isEmpty()
                                 ? m_data->key()
                                 : m_data->info().displayName(),
                             this);
    QFont nameFont = m_nameLabel->font();
    nameFont.setBold(true);
    m_nameLabel->setFont(nameFont);
    m_defaultTextColor = m_nameLabel->palette().color(QPalette::WindowText);
    titleLayout->addWidget(m_nameLabel);
    titleLayout->addStretch();

    // 重置按钮
    m_resetButton = new QToolButton(this);
    m_resetButton->setText("🔄");
    m_resetButton->setFixedSize(24,24);
    m_resetButton->setToolTip(tr("Reset to default value"));
    m_resetButton->setVisible(!m_data->info().defaultValue().isNull());
    connect(m_resetButton, &QToolButton::clicked, this,
            &WConfigItemWidget::onResetToDefault);
    titleLayout->addWidget(m_resetButton);

    mainLayout->addLayout(titleLayout);

    // 描述
    m_descLabel = new QLabel(m_data->info().description(), this);
    QFont descFont = m_descLabel->font();
    descFont.setPointSize(descFont.pointSize() - 1);
    m_descLabel->setFont(descFont);
    m_descLabel->setStyleSheet("color: #666666;");
    m_descLabel->setObjectName("DescLabel");
    mainLayout->addWidget(m_descLabel);

    m_undoButton = new QToolButton(this);
    m_undoButton->setText("⬅");
    m_undoButton->setFixedSize(24,24);
    m_undoButton->setToolTip(tr("Undo changes"));
    m_undoButton->setVisible(false);
    connect(m_undoButton, &QToolButton::clicked, this,
            &WConfigItemWidget::onUndo);
    titleLayout->addWidget(m_undoButton);

    // 编辑器
    m_editor = createValueWidget();
    m_editor->createEditor();
    m_editor->setConfigData(m_data);
    connect(m_editor, &WConfigEditorBase::valueChanged, this,
            &WConfigItemWidget::updateUndoVisibility);
    mainLayout->addWidget(m_editor);

    if (m_data->hasProperty(Property::ReadOnly)) {
        m_editor->setEnabled(false);
        m_resetButton->setEnabled(false);
    }

    setFocusPolicy(Qt::ClickFocus);

    m_itemRef = QSharedPointer<WConfigItemRef>::create(m_data);
    m_itemRef->setOnChanged([this]() { updateUndoVisibility(); });
    updateUndoVisibility();
    connect(m_editor, &WConfigEditorBase::valueChanged, this,
            &WConfigItemWidget::valueChanged);
}

WConfigEditorBase *WConfigItemWidget::createValueWidget() {
    switch (m_data->type()) {
    case DataType::Int:
        return new WConfigEditorInt(this);
    case DataType::Double:
        return new WConfigEditorDouble(this);
    case DataType::String:
        return new WConfigEditorString(this);
    case DataType::Bool:
        return new WConfigEditorBool(this);
    case DataType::Array:
        return new WConfigEditorArray(this);
    case DataType::Object:
        return new WConfigEditorObject(this);
    case DataType::Select:
        return new WConfigEditorSelect(this);
    case DataType::Action:
        return new WConfigEditorAction(this);
    default:
        return nullptr;
    }
}

void WConfigItemWidget::updateUndoVisibility() {
    if (m_data) {
        bool modified = (m_data->getTemporary() != m_data->getPersistent());
        m_undoButton->setVisible(modified);
    }
}

void WConfigItemWidget::onUndo() {
    if (m_data) {
        m_data->revertToPersistent();
        if (m_editor) m_editor->setConfigData(m_data);
        updateUndoVisibility();
        emit valueChanged();
    }
}

QVariant WConfigItemWidget::currentValue() const {
    return m_editor ? m_editor->configData()->toVariant() : QVariant();
}

void WConfigItemWidget::setCurrentValue(const QVariant &value) {
    if (m_editor && value.isValid())
        m_editor->setConfigValue(value);
}

void WConfigItemWidget::setSelected(bool selected)
{
    if (!m_nameLabel) return;
    // QPalette pal = m_nameLabel->palette();
    if (selected)
        m_nameLabel->setStyleSheet("color: #89b2b8ff;");
    //pal.setColor(QPalette::WindowText, Qt::blue);
    else
        m_nameLabel->setStyleSheet("");
    //pal.setColor(QPalette::WindowText, m_defaultTextColor);
    //m_nameLabel->setPalette(pal);
}

void WConfigItemWidget::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    emit clicked(this);
}

void WConfigItemWidget::onResetToDefault() {
    if (!m_data)
        return;
    QVariant defaultVal = m_data->info().defaultValue();
    if (!defaultVal.isValid()) {
        QMessageBox::information(this, tr("No Default"),
                                 tr("This item has no default value."));
        return;
    }
    m_data->setTemporary(defaultVal);
    if (m_editor) {
        m_editor->setConfigData(m_data);
    }
    updateUndoVisibility();
    emit valueChanged();
}

} // namespace we::config