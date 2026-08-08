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
#include "WECore/config/WConfigDataDouble.h"
#include <cmath>

namespace we::config {

WConfigDataDouble::WConfigDataDouble() { m_type = DataType::Double; }

WConfigDataBase *WConfigDataDouble::init(const QString &key,
                                         const WConfigItemInfo &info,
                                         WConfigViewer *parent) {
    WConfigDataBase::init(DataType::Double, key, info, parent);
    m_value = info.defaultValue().toDouble();
    m_original = m_value;
    return this;
}

WConfigDataDouble *WConfigDataDouble::init(const QString &key,
                                           double defaultValue,
                                           int decimalPlaces,
                                           const Properties &properties,
                                           WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue).decimalPlaces(decimalPlaces);
    for (Property p : properties)
        info.property(p);
    init(key, info, parent);
    return this;
}

bool WConfigDataDouble::setTemporary(double value) {
    int places = m_info.decimalPlaces();
    double factor = std::pow(10.0, places);
    double rounded = std::round(value * factor) / factor;
    if (qFuzzyCompare(m_value, rounded))
        return false;
    m_value = rounded;
    return true;
}
bool WConfigDataDouble::setTemporary(const QVariant &value) {
    return setTemporary(value.toDouble());
}

bool WConfigDataDouble::setPersistent(const QVariant &val, bool emitSignal) {
    double newVal = val.toDouble();
    if (qFuzzyCompare(m_original, newVal)) return false;
    m_original = newVal;
    if (emitSignal)notifyChange();
    return true;
}

QVariant WConfigDataDouble::toVariant() const { return m_value; }
bool WConfigDataDouble::fromVariant(const QVariant &variant) {
    return setTemporary(variant.toDouble());
}

} // namespace we::config