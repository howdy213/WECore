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