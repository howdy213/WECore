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
#include "WECore/config/WConfigEditorBool.h"
#include <QVBoxLayout>

namespace we::config {

WConfigEditorBool::WConfigEditorBool(WConfigItemWidget* parent) : WConfigEditorBase(parent) { m_type = DataType::Bool; }

void WConfigEditorBool::createEditor()
{
    if (m_boolCheck) return;
    m_boolCheck = new QCheckBox(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_boolCheck);
    setLayout(layout);
    connect(m_boolCheck, &QCheckBox::toggled, this, [this](bool checked) {
        if (m_data) {
            bool oldVal = m_data->getTemporary().toBool();
            if (checked != oldVal) {
                m_data->setTemporary(checked);
                emit valueChanged();
            }
        }
    });
}

void WConfigEditorBool::setConfigData(WConfigDataBase* data)
{
    m_data = data;
    auto* boolData = static_cast<WConfigDataBool*>(data);
    if (boolData) m_boolCheck->setChecked(boolData->toVariant().toBool());
}

WConfigDataBool* WConfigEditorBool::getData()
{
    bool newVal = m_boolCheck->isChecked();
    bool oldVal = m_data->getTemporary().toBool();
    if (newVal != oldVal) {
        m_data->setTemporary(newVal);
        emit valueChanged();
    }
    return static_cast<WConfigDataBool*>(m_data);
}

WConfigDataBase* WConfigEditorBool::configData() { return getData(); }

} // namespace we::config