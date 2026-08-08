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
#include "WECore/config/WConfigEditorBase.h"
#include "WECore/config/WConfigItemWidget.h"

namespace we::config {

WConfigEditorBase::WConfigEditorBase(WConfigItemWidget* parent) : QWidget(parent) {}

void WConfigEditorBase::setConfigValue(QVariant value) { if (m_data) m_data->setTemporary(value); }
void WConfigEditorBase::setConfigData(WConfigDataBase* data) { if (data && data->type() == m_type) m_data = data; }
WConfigDataBase* WConfigEditorBase::configData() { return m_data; }
void WConfigEditorBase::createEditor() {}

QWidget *NoEditColumnDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.column() == 0)
        return nullptr;
    return QStyledItemDelegate::createEditor(parent, option, index);
}

} // namespace we::config