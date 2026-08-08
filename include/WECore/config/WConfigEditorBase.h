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
#ifndef WCONFIGEDITORBASE_H
#define WCONFIGEDITORBASE_H

#include "WConfigDataBase.h"
#include <QStyledItemDelegate>
#include <QWidget>

namespace we::config {

class NoEditColumnDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
};

class WConfigItemWidget;

class WE_EXPORT WConfigEditorBase : public QWidget {
    Q_OBJECT
public:
    WConfigEditorBase(WConfigItemWidget *parent = nullptr);

    virtual void setConfigValue(QVariant value);
    virtual void setConfigData(WConfigDataBase *data);
    virtual WConfigDataBase *configData();
    virtual void createEditor();

signals:
    void valueChanged();  // 当编辑器值被用户修改时发射
protected:
    WConfigDataBase *m_data = nullptr;
    DataType m_type = DataType::None;
};

} // namespace we::config

#endif // WCONFIGEDITORBASE_H