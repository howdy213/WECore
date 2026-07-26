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