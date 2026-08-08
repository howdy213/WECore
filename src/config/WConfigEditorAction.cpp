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
#include "WECore/config/WConfigEditorAction.h"
#include "WECore/config/WConfigDataAction.h"
#include <QHBoxLayout>

namespace we::config {

WConfigEditorAction::WConfigEditorAction(WConfigItemWidget* parent) : WConfigEditorBase(parent) { m_type = DataType::Action; }

void WConfigEditorAction::createEditor()
{
    m_button = new QPushButton(this);
    if (m_data) {
        m_button->setText(m_data->info().displayName().isEmpty() ? m_data->key() : m_data->info().displayName());
    }
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addStretch();
    layout->addWidget(m_button);
    setLayout(layout);
    connect(m_button, &QPushButton::clicked, this, &WConfigEditorAction::onButtonClicked);
}

void WConfigEditorAction::setConfigData(WConfigDataBase* data)
{
    m_data = data;
    m_actionData = static_cast<WConfigDataAction*>(data);
    if (m_button) {
        m_button->setText(m_data->info().displayName().isEmpty() ? m_data->key() : m_data->info().displayName());
    }
}

WConfigDataBase* WConfigEditorAction::configData() { return m_actionData; }

void WConfigEditorAction::onButtonClicked() { if (m_actionData) m_actionData->execute(); }

} // namespace we::config