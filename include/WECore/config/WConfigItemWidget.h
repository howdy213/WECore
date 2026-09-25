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
#ifndef WCONFIGITEMWIDGET_H
#define WCONFIGITEMWIDGET_H

#include "WConfigDataBase.h"
#include "WConfigEditorBase.h"
#include <QLabel>
#include <QToolButton>
#include <QVariant>
#include <QWidget>

namespace we::config {

// Full shows a title row + description; Compact is for groups/inline and shows
// only the editor; Inline places the title to the left, on the same row as the
// editor (reset/undo buttons on the right).
enum class WConfigItemWidgetStyle { Full, Compact, Inline };

// Renders a single config item: title, description, the type-specific editor and
// reset/undo buttons, per the chosen WConfigItemWidgetStyle.
class WE_EXPORT WConfigItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit WConfigItemWidget(WConfigDataBase *data,
                               WConfigItemWidgetStyle style =
                                   WConfigItemWidgetStyle::Full,
                               QWidget *parent = nullptr);

    WConfigDataBase *configData() const { return m_data; }
    QVariant currentValue() const;
    void setCurrentValue(const QVariant &value);
    void setSelected(bool selected);

    // Accessors for item-level layout adjusters (each may be nullptr):
    // - title label (present for Full/Inline; none for Compact)
    // - value editor (scalar/custom editors; none for Action, special for Array/Object)
    // - description label (Full only)
    QLabel *titleWidget() const { return m_nameLabel; }
    WConfigEditorBase *valueEditor() const { return m_editor; }
    QLabel *descriptionWidget() const { return m_descLabel; }

    // Visibility control (used to suppress title/description in merged groups).
    void setShowTitle(bool show);
    void setShowDescription(bool show);
    // Write displayName + description into the tooltip (for flattened display).
    void setStyleTooltip();
    // Caller-driven refresh: editor re-reads data, read-only re-applied, undo visibility updated.
    void refresh();
signals:
    void clicked(we::config::WConfigItemWidget *widget);
    void valueChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onUndo();
    void onResetToDefault();

private:
    WConfigEditorBase *createValueWidget();
    void createButtons(); // Create the reset/undo buttons (shared by Full's title row and Inline)
    void updateUndoVisibility();
    void applyReadOnly();
    void applyTitleVisibility();
    QLabel *m_nameLabel = nullptr;
    QLabel *m_descLabel = nullptr;
    QToolButton *m_resetButton = nullptr;
    WConfigDataBase *m_data = nullptr;
    WConfigEditorBase *m_editor = nullptr;
    QToolButton *m_undoButton = nullptr;
    QSharedPointer<WConfigItemRef> m_itemRef;
    bool m_showTitle = true;
    bool m_showDesc = true;
};

} // namespace we::config

#endif // WCONFIGITEMWIDGET_H