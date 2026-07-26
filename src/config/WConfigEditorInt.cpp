#include "WECore/config/WConfigEditorInt.h"
#include <QVBoxLayout>
#include <limits>

namespace we::config {

WConfigEditorInt::WConfigEditorInt(WConfigItemWidget *parent)
    : WConfigEditorBase(parent) {
    m_type = DataType::Int;
}

void WConfigEditorInt::createEditor() {
    m_intSpin = new QSpinBox(this);
    m_intSpin->setMinimum(std::numeric_limits<int>::min());
    m_intSpin->setMaximum(std::numeric_limits<int>::max());
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_intSpin);
    setLayout(layout);
    connect(m_intSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
        if (m_data) {
            int oldVal = m_data->getTemporary().toInt();
            if (value != oldVal) {
                m_data->setTemporary(value);
                emit valueChanged();
            }
        }
    });
}

void WConfigEditorInt::setConfigData(WConfigDataBase *data) {
    m_data = data;
    auto *intData = static_cast<WConfigDataInt *>(data);
    if (intData)
        m_intSpin->setValue(intData->toVariant().toInt());
}

WConfigDataInt* WConfigEditorInt::getData()
{
    int newVal = m_intSpin->value();
    int oldVal = m_data->getTemporary().toInt();
    if (newVal != oldVal) {
        m_data->setTemporary(newVal);
        emit valueChanged();
    }
    return static_cast<WConfigDataInt*>(m_data);
}

WConfigDataBase *WConfigEditorInt::configData() { return getData(); }

} // namespace we::config