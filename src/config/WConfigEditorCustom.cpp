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
#include "WECore/config/WConfigEditorCustom.h"
#include "WECore/config/WConfigDataCustom.h"
#include <QVBoxLayout>

namespace we::config {

WConfigEditorCustom::WConfigEditorCustom(WConfigItemWidget *parent)
    : WConfigEditorBase(parent) {
    m_type = DataType::Custom;
}

void WConfigEditorCustom::createEditor() {
    // Lazy build: the type name is only known in setConfigData, so defer building until then.
}

void WConfigEditorCustom::setConfigData(WConfigDataBase *data) {
    m_data = data;
    auto *customData = dynamic_cast<WConfigDataCustom *>(data);
    if (customData && !m_built)
        buildFromType(customData->typeName());
    refreshFromData();
}

void WConfigEditorCustom::buildFromType(const QString &typeName) {
    const WConfigCustomType *t =
        WConfigCustomTypeRegistry::instance().type(typeName);
    if (!t || !t->editorFactory)
        return;
    m_customWidget = t->editorFactory(this, &m_iface);
    if (!m_customWidget)
        return;
    m_built = true;

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_customWidget);
    setLayout(layout);

    connect(m_customWidget, SIGNAL(valueEdited()), this,
            SLOT(onCustomValueEdited()));
}

WConfigDataBase *WConfigEditorCustom::configData() {
    // Flush the current UI value into the data (refreshFromData takes the reverse path).
    if (m_data && m_iface) {
        if (m_data->setTemporary(m_iface->editValue()))
            emit valueChanged();
    }
    return m_data;
}

void WConfigEditorCustom::refreshFromData() {
    if (m_data && m_iface)
        m_iface->setEditValue(m_data->getTemporary());
}

void WConfigEditorCustom::onCustomValueEdited() {
    if (m_data && m_iface) {
        if (m_data->setTemporary(m_iface->editValue()))
            emit valueChanged();
    }
}

} // namespace we::config