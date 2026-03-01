/**
 * @file wconfig.h
 * @brief 配置模板类头文件
 * @author howdy213
 * @date 2026-1-30
 * @version 1.1.0
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
#ifndef WCONFIG_H
#define WCONFIG_H
#include "../WDef/wedef.h"

#include <QString>

template <class T> class WConfigPrivate {
public:
    QMap<QString, T> map;
    QMap<QString, T> defaultMap;
    T defaultValue;
};

template <class T> class WE_EXPORT WE_NAMESPACE::WConfig {
public:
    WConfig();
    virtual ~WConfig();
    T get(QString key);
    bool setDefault(QString key, T value);
    bool hasArg(QString key);
    bool set(QString key, T value);
    void setDefaultValue(T value);
    QMap<QString, T> toMap();
    QMap<QString, T> toMapDefualt();

private:
    WConfigPrivate<T> *d = nullptr;
};

///
/// \brief WConfig::WConfig
///
template <class T> WConfig<T>::WConfig() { this->d = new WConfigPrivate<T>; }
///
/// \brief WConfig::~WConfig
///
template <class T> WConfig<T>::~WConfig() { delete this->d; }
///
/// \brief WConfig::get
/// \param key
/// \return
///
template <class T> T WConfig<T>::get(QString key) {
    if (!hasArg(key))
        return d->defaultValue;
    else
        return d->map.contains(key) ? d->map[key] : d->defaultMap[key];
}
///
/// \brief WConfig::setDefault
/// \param key
/// \param value
/// \return
///
template <class T> bool WConfig<T>::setDefault(QString key, T value) {
    d->defaultMap[key] = value;
    return !d->map.contains(key);
}
///
/// \brief WConfig::hasArg
/// \param key
/// \return
///
template <class T> bool WConfig<T>::hasArg(QString key) {
    return d->defaultMap.contains(key) || d->map.contains(key);
}
///
/// \brief WConfig::set
/// \param key
/// \param value
/// \return
///
template <class T> bool WConfig<T>::set(QString key, T value) {
    bool res = false;
    if (hasArg(key))
        res = true;
    d->map.insert(key, value);
    return res;
}
///
/// \brief WConfig::setDefaultValue
/// \param value
///
template <class T> void WConfig<T>::setDefaultValue(T value) {
    d->defaultValue = value;
}
///
/// \brief WConfig::toMap
/// \return
///
template <class T> QMap<QString, T> WConfig<T>::toMap() { return d->map; }
///
/// \brief WConfig::toMapDefualt
/// \return
///
template <class T> QMap<QString, T> WConfig<T>::toMapDefualt() {
    return d->defaultMap;
}
#endif // WCONFIG_H
