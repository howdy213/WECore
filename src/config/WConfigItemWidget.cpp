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
#include "WECore/config/WConfigItemWidget.h"
#include "WECore/config/WConfigEditorAction.h"
#include "WECore/config/WConfigEditorArray.h"
#include "WECore/config/WConfigEditorBool.h"
#include "WECore/config/WConfigEditorCustom.h"
#include "WECore/config/WConfigEditorDouble.h"
#include "WECore/config/WConfigEditorInt.h"
#include "WECore/config/WConfigEditorObject.h"
#include "WECore/config/WConfigEditorSelect.h"
#include "WECore/config/WConfigEditorString.h"
#include "WECore/config/WConfigLayout.h"
#include "WECore/config/WConfigRef.h"
#include <QFont>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStyle>
#include <QVBoxLayout>

namespace we::config {

WConfigItemWidget::WConfigItemWidget(WConfigDataBase *data,
                                     WConfigItemWidgetStyle style, QWidget *parent)
    : QWidget(parent), m_data(data) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(Layout::ItemSpacing);
    mainLayout->setContentsMargins(Layout::ContentsMargin, Layout::ContentsMargin,
                                   Layout::ContentsMargin, Layout::ContentsMargin);

    const bool isFull = (style == WConfigItemWidgetStyle::Full);
    const bool isInline = (style == WConfigItemWidgetStyle::Inline);

    if (isFull) {
        // Title row: name + reset + undo buttons.
        QHBoxLayout *titleLayout = new QHBoxLayout();
        titleLayout->setSpacing(Layout::RowSpacing);
        m_nameLabel = new QLabel(m_data->info().displayName().isEmpty()
                                     ? m_data->key()
                                     : m_data->info().displayName(),
                                 this);
        QFont nameFont = m_nameLabel->font();
        nameFont.setBold(true);
        m_nameLabel->setFont(nameFont);
        m_nameLabel->setWordWrap(true); // Wrap long titles so they don't squeeze the right-side buttons
        titleLayout->addWidget(m_nameLabel);
        titleLayout->addStretch();

        createButtons();
        titleLayout->addWidget(m_resetButton);
        titleLayout->addWidget(m_undoButton);

        mainLayout->addLayout(titleLayout);

        // Description (hidden when empty).
        m_descLabel = new QLabel(m_data->info().description(), this);
        QFont descFont = m_descLabel->font();
        descFont.setPointSize(descFont.pointSize() - 1);
        m_descLabel->setFont(descFont);
        m_descLabel->setStyleSheet("color: #666666;");
        m_descLabel->setObjectName("DescLabel");
        m_descLabel->setWordWrap(true); // Wrap long descriptions to avoid horizontal overflow
        m_descLabel->setVisible(!m_data->info().description().isEmpty());
        mainLayout->addWidget(m_descLabel);
    }

    m_editor = createValueWidget();
    if (m_editor) {
        m_editor->createEditor();
        m_editor->setConfigData(m_data);
        connect(m_editor, &WConfigEditorBase::valueChanged, this,
                &WConfigItemWidget::updateUndoVisibility);
    }

    if (isFull) {
        if (m_editor)
            mainLayout->addWidget(m_editor);
    } else if (isInline) {
        // Inline: title on the left, editor in the middle, reset/undo on the right.
        QHBoxLayout *inlineLayout = new QHBoxLayout();
        inlineLayout->setContentsMargins(0, 0, 0, 0);
        inlineLayout->setSpacing(Layout::RowSpacing);
        m_nameLabel = new QLabel(m_data->info().displayName().isEmpty()
                                     ? m_data->key()
                                     : m_data->info().displayName(),
                                 this);
        QFont nameFont = m_nameLabel->font();
        nameFont.setBold(true);
        m_nameLabel->setFont(nameFont);
        m_nameLabel->setMinimumWidth(90); // Fixed title width keeps multiple Inline rows aligned
        inlineLayout->addWidget(m_nameLabel);
        if (m_editor) {
            // Let the editor shrink: spin boxes have a large intrinsic minimum width and
            // would break the row when several Inline items sit side by side.
            m_editor->setSizePolicy(QSizePolicy::Ignored,
                                    m_editor->sizePolicy().verticalPolicy());
            m_editor->setMinimumWidth(40);
            inlineLayout->addWidget(m_editor, 1);
        }
        createButtons();
        inlineLayout->addWidget(m_resetButton);
        inlineLayout->addWidget(m_undoButton);
        mainLayout->addLayout(inlineLayout);
    } else {
        // Compact: editor + reset/undo buttons on the right.
        QHBoxLayout *compactLayout =
            new QHBoxLayout();
        compactLayout->setContentsMargins(0, 0, 0, 0);
        compactLayout->setSpacing(Layout::RowSpacing);
        if (m_editor)
            compactLayout->addWidget(m_editor, 1);
        createButtons();
        compactLayout->addWidget(m_resetButton);
        compactLayout->addWidget(m_undoButton);
        mainLayout->addLayout(compactLayout);
    }

    applyReadOnly();

    if (isInline) {
        // Inline has no separate description row; the tooltip carries the details.
        setStyleTooltip();
        // Title row is shown by default (works with setShowTitle).
        applyTitleVisibility();
    } else if (!m_data->info().description().isEmpty()) {
        setStyleTooltip();
    }

    setFocusPolicy(Qt::ClickFocus);

    m_itemRef = QSharedPointer<WConfigItemRef>::create(m_data);
    m_itemRef->setOnChanged([this]() { updateUndoVisibility(); });
    updateUndoVisibility();
    if (m_editor) {
        connect(m_editor, &WConfigEditorBase::valueChanged, this,
                &WConfigItemWidget::valueChanged);
    }
}

void WConfigItemWidget::createButtons() {
    if (m_resetButton || m_undoButton)
        return;
    m_resetButton = new QToolButton(this);
    m_resetButton->setText(QStringLiteral("🔄"));
    m_resetButton->setFixedSize(24, 24);
    m_resetButton->setToolTip(tr("Reset to default value"));
    m_resetButton->setVisible(!m_data->info().defaultValue().isNull());
    connect(m_resetButton, &QToolButton::clicked, this,
            &WConfigItemWidget::onResetToDefault);

    m_undoButton = new QToolButton(this);
    m_undoButton->setText(QStringLiteral("⬅"));
    m_undoButton->setFixedSize(24, 24);
    m_undoButton->setToolTip(tr("Undo changes"));
    m_undoButton->setVisible(false);
    connect(m_undoButton, &QToolButton::clicked, this,
            &WConfigItemWidget::onUndo);
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
    case DataType::Custom:
        return new WConfigEditorCustom(this);
    default:
        return nullptr;
    }
}

void WConfigItemWidget::applyReadOnly() {
    if (!m_editor)
        return;
    bool ro = m_data && m_data->hasProperty(Property::ReadOnly);
    m_editor->setEnabled(!ro);
    if (m_resetButton)
        m_resetButton->setEnabled(!ro);
}

void WConfigItemWidget::setShowTitle(bool show) {
    m_showTitle = show;
    applyTitleVisibility();
}

void WConfigItemWidget::setShowDescription(bool show) {
    m_showDesc = show;
    if (m_descLabel)
        m_descLabel->setVisible(show && !m_descLabel->text().isEmpty());
}

void WConfigItemWidget::applyTitleVisibility() {
    if (m_nameLabel)
        m_nameLabel->setVisible(m_showTitle);
    if (m_resetButton)
        m_resetButton->setVisible(m_showTitle &&
                                  !m_data->info().defaultValue().isNull());
    if (m_undoButton) {
        bool actModified = m_data &&
                           (m_data->getTemporary() != m_data->getPersistent());
        m_undoButton->setVisible(m_showTitle && actModified);
    }
}

void WConfigItemWidget::setStyleTooltip() {
    if (!m_data)
        return;
    QString tip = m_data->info().displayName().isEmpty() ? m_data->key()
                                                         : m_data->info().displayName();
    if (!m_data->info().description().isEmpty())
        tip += "\n\n" + m_data->info().description();
    setToolTip(tip);
}

void WConfigItemWidget::refresh() {
    if (m_editor) {
        m_editor->refreshFromData();
        m_editor->setConfigData(m_data);
    }
    applyReadOnly();
    updateUndoVisibility();
}

void WConfigItemWidget::updateUndoVisibility() {
    if (m_data && m_undoButton) {
        bool actModified = (m_data->getTemporary() != m_data->getPersistent());
        m_undoButton->setVisible(m_showTitle && actModified);
    }
}

void WConfigItemWidget::onUndo() {
    if (m_data) {
        m_data->revertToPersistent();
        if (m_editor)
            m_editor->setConfigData(m_data);
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
    if (selected)
        m_nameLabel->setStyleSheet("color: #89b2b8ff;");
    else
        m_nameLabel->setStyleSheet("");
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