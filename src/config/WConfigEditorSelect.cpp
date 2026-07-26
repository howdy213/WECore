#include "WECore/config/WConfigEditorSelect.h"
#include <QHBoxLayout>

namespace we::config {

WConfigEditorSelect::WConfigEditorSelect(WConfigItemWidget *parent)
    : WConfigEditorBase(parent) {
    m_type = DataType::Select;
}

void WConfigEditorSelect::createEditor() {
    if (m_combo)
        return;
    m_combo = new QComboBox(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_combo);
    setLayout(layout);
    if (m_data) {
        auto *selectData = static_cast<WConfigDataSelect *>(m_data);
        m_combo->addItems(selectData->options());
        m_combo->setCurrentText(m_data->getTemporary().toString());
        if (m_data->hasProperty(Property::ReadOnly))
            m_combo->setEnabled(false);
    }
    connect(m_combo, &QComboBox::currentTextChanged, this,
            [this](const QString &text) {
                if (m_data) {
            QString oldVal = m_data->getTemporary().toString();
                    if (text != oldVal) {
                m_data->setTemporary(text);
                emit valueChanged();
                    }
                }
    });
}

void WConfigEditorSelect::setConfigData(WConfigDataBase *data) {
    m_data = data;
    auto *selectData = static_cast<WConfigDataSelect *>(data);
    QString temp = selectData->getTemporary().toString();
    m_combo->clear();
    m_combo->addItems(selectData->options());
    m_combo->setCurrentText(temp);
    m_combo->setEnabled(!m_data->hasProperty(Property::ReadOnly));
}

WConfigDataSelect *WConfigEditorSelect::getData() {
    QString newVal = m_combo->currentText();
    if (newVal != m_data->getTemporary().toString()) {
        m_data->setTemporary(newVal);
        emit valueChanged();
    }
    return static_cast<WConfigDataSelect *>(m_data);
}

WConfigDataBase *WConfigEditorSelect::configData() { return getData(); }

} // namespace we::config