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
#ifndef WCONFIGEDITORBASE_H
#define WCONFIGEDITORBASE_H

#include "WConfigDataBase.h"
#include <QStyledItemDelegate>
#include <QWidget>

namespace we::config {

// Delegate that blocks editing in column 0 (the key/index column).
class NoEditColumnDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
};

class WConfigItemWidget;

// Base class for per-type value editors. Renders one config item into a widget and
// writes user edits back into the item's temporary value. Editors are created as
// children of their owning WConfigItemWidget.
class WE_EXPORT WConfigEditorBase : public QWidget {
    Q_OBJECT
public:
    WConfigEditorBase(WConfigItemWidget *parent = nullptr);

    virtual void setConfigValue(QVariant value);
    virtual void setConfigData(WConfigDataBase *data);
    virtual WConfigDataBase *configData();
    virtual void createEditor();
    // Caller-driven refresh: read the value from m_data back into the widget
    // (default no-op).
    virtual void refreshFromData();

signals:
    void valueChanged(); // Emitted when the user modifies the editor value.
protected:
    WConfigDataBase *m_data = nullptr;
    DataType m_type = DataType::None; // Expected item type, set by each subclass ctor
};

} // namespace we::config

#endif // WCONFIGEDITORBASE_H