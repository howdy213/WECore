#include "WECore/config/WConfigEditorDouble.h"
#include <QVBoxLayout>

namespace we::config {

WConfigEditorDouble::WConfigEditorDouble(WConfigItemWidget *parent)
    : WConfigEditorBase(parent) {
    m_type = DataType::Double;
}

void WConfigEditorDouble::createEditor() {
    m_doubleSpin = new QDoubleSpinBox(this);
    m_doubleSpin->setMinimum(-1e20);
    m_doubleSpin->setMaximum(1e20);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_doubleSpin);
    setLayout(layout);
    connect(m_doubleSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double value) {
        if (m_data) {
            double oldVal = m_data->getTemporary().toDouble();
            if (!qFuzzyCompare(value, oldVal)) {
                m_data->setTemporary(value);
                emit valueChanged();
            }
        }
    });
}

void WConfigEditorDouble::setConfigData(WConfigDataBase *data) {
    auto *doubleData = static_cast<WConfigDataDouble *>(data);
    if (doubleData) {
        m_data = data;
        int decimals = doubleData->decimalPlaces();
        m_doubleSpin->setDecimals(decimals);
        m_doubleSpin->setValue(doubleData->toVariant().toDouble());
    }
}

WConfigDataDouble *WConfigEditorDouble::getData() {
    double newVal = m_doubleSpin->value();
    double oldVal = m_data->getTemporary().toDouble();
    if (!qFuzzyCompare(newVal, oldVal)) {
        m_data->setTemporary(newVal);
        emit valueChanged();
    }
    return static_cast<WConfigDataDouble *>(m_data);
}

WConfigDataBase *WConfigEditorDouble::configData() { return getData(); }

} // namespace we::config