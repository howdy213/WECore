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
#include "WECore/config/WConfigEditorString.h"
#include <QVBoxLayout>

namespace we::config {

WConfigEditorString::WConfigEditorString(WConfigItemWidget* parent) : WConfigEditorBase(parent) { m_type = DataType::String; }

void WConfigEditorString::createEditor()
{
    if (m_stringEdit) return;
    m_stringEdit = new QLineEdit(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_stringEdit);
    setLayout(layout);
    connect(m_stringEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        if (m_data) {
            QString oldVal = m_data->getTemporary().toString();
            if (text != oldVal) {
                m_data->setTemporary(text);
                emit valueChanged();
            }
        }
    });
}

void WConfigEditorString::setConfigData(WConfigDataBase* data)
{
    m_data = data;
    auto* strData = static_cast<WConfigDataString*>(data);
    if (strData) m_stringEdit->setText(strData->toVariant().toString());
}

WConfigDataString* WConfigEditorString::getData()
{
    QString newVal = m_stringEdit->text();
    if (newVal != m_data->getTemporary().toString()) {
        m_data->setTemporary(newVal);
        emit valueChanged();
    }
    return static_cast<WConfigDataString*>(m_data);
}
WConfigDataBase* WConfigEditorString::configData() { return getData(); }

} // namespace we::config