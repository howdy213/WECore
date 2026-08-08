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
#ifndef WCONFIGITEMWIDGET_H
#define WCONFIGITEMWIDGET_H

#include "WConfigDataBase.h"
#include "WConfigEditorBase.h"
#include <QLabel>
#include <QToolButton>
#include <QVariant>
#include <QWidget>

namespace we::config {

class WE_EXPORT WConfigItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit WConfigItemWidget(WConfigDataBase *data, QWidget *parent = nullptr);

    WConfigDataBase *configData() const { return m_data; }
    QVariant currentValue() const;
    void setCurrentValue(const QVariant &value);
    void setSelected(bool selected);
signals:
    void clicked(we::config::WConfigItemWidget *widget);
    void valueChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onUndo();
    void onResetToDefault();

private:
    QColor m_defaultTextColor;
    WConfigEditorBase *createValueWidget();
    void updateUndoVisibility();
    QLabel *m_nameLabel = nullptr;
    QLabel *m_descLabel = nullptr;
    QToolButton *m_resetButton = nullptr;
    WConfigDataBase *m_data = nullptr;
    WConfigEditorBase *m_editor = nullptr;
    QToolButton *m_undoButton = nullptr;
    QSharedPointer<WConfigItemRef> m_itemRef;
};

} // namespace we::config

#endif // WCONFIGITEMWIDGET_H