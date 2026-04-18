/**
 * @file wconfig.h
 * @brief Configuration template class header
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
#include <QMap>
#include <QString>

template <class T>
class WConfigPrivate {
public:
    QMap<QString, T> map;
    QMap<QString, T> defaultMap;
    T defaultValue;
};

template <class T>
class WE_EXPORT WE_NAMESPACE::WConfig {
public:
    WConfig();
    virtual ~WConfig();
    Q_DISABLE_COPY(WConfig)

    /**
     * @brief Retrieve value for a key.
     * @return Value from user map if present, otherwise per-key default,
     *         finally global default value.
     */
    T get(const QString& key) const;

    /**
     * @brief Set a per-key default value.
     * @return false if the key already has a user-set value, true otherwise.
     */
    bool setDefault(const QString& key, const T& value);

    bool hasArg(const QString& key) const;
    bool set(const QString& key, const T& value);
    void setDefaultValue(const T& value);

    QMap<QString, T> toMap() const;
    QMap<QString, T> toMapDefault() const;

protected:
    WConfigPrivate<T> *d = nullptr;
};

// ---------------------- Implementation ----------------------

template <class T>
WConfig<T>::WConfig() {
    d = new WConfigPrivate<T>;
}

template <class T>
WConfig<T>::~WConfig() {
    delete d;
}

template <class T>
T WConfig<T>::get(const QString& key) const {
    if (d->map.contains(key)) {
        return d->map[key];
    }
    if (d->defaultMap.contains(key)) {
        return d->defaultMap[key];
    }
    return d->defaultValue;
}

template <class T>
bool WConfig<T>::setDefault(const QString& key, const T& value) {
    d->defaultMap[key] = value;
    return !d->map.contains(key);
}

template <class T>
bool WConfig<T>::hasArg(const QString& key) const {
    return d->map.contains(key) || d->defaultMap.contains(key);
}

template <class T>
bool WConfig<T>::set(const QString& key, const T& value) {
    const bool existed = hasArg(key);
    d->map.insert(key, value);
    return existed;
}

template <class T>
void WConfig<T>::setDefaultValue(const T& value) {
    d->defaultValue = value;
}

template <class T>
QMap<QString, T> WConfig<T>::toMap() const {
    return d->map;
}

template <class T>
QMap<QString, T> WConfig<T>::toMapDefault() const {
    return d->defaultMap;
}

#endif // WCONFIG_H