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