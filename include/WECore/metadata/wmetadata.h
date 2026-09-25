/**
 * @file wmetadata.h
 * @brief Template class for key‑value metadata storage with defaults.
 *
 * WMetaData provides a dictionary‑like container that supports per‑key
 * defaults, a global fallback value, and import/export via QMap.
 *
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
#ifndef WMETADATA_H
#define WMETADATA_H

#include <QMap>
#include <QScopedPointer>
#include <QString>

#include "WECore/def/wedef.h"

namespace we {

/// Private data of WMetaData (d‑pointer pattern).
template <class T>
class WConfigPrivate
{
public:
    QMap<QString, T> map;         ///< User‑set key‑value pairs.
    QMap<QString, T> defaultMap;  ///< Per‑key fallback values.
    T defaultValue;               ///< Global fallback value.
};

/**
 * @brief Key‑value container with per‑key and global defaults.
 *
 * get() resolves a key in order: user‑set value, per‑key default,
 * then the global default.
 */
template <class T>
class WE_EXPORT WMetaData
{
    Q_DISABLE_COPY(WMetaData)

public:
    WMetaData();

    virtual ~WMetaData();

    /// Returns the user value for @p key, else its per‑key default, else the global default.
    T get(const QString &key) const;

    /// Sets the per‑key default; returns @c false if a user value already overrides it.
    bool setDefault(const QString &key, const T &value);

    /// True if @p key has a user value or a per‑key default.
    bool hasArg(const QString &key) const;

    /// Stores a user value; returns @c true if the key already existed (user value or default).
    bool set(const QString &key, const T &value);

    /// Sets the global fallback returned by get() when the key has no value or per‑key default.
    void setDefaultValue(const T &value);

    /// Returns a copy of all user‑set key‑value pairs.
    QMap<QString, T> toMap() const;

    /// Returns a copy of all per‑key defaults.
    QMap<QString, T> toMapDefault() const;

protected:
    QScopedPointer<WConfigPrivate<T>> d; ///< Private data (d‑pointer).
};


// Template implementation


template <class T>
WMetaData<T>::WMetaData()
    : d(new WConfigPrivate<T>)
{
}

template <class T>
WMetaData<T>::~WMetaData() = default;

template <class T>
T WMetaData<T>::get(const QString &key) const
{
    if (d->map.contains(key))
        return d->map[key];
    if (d->defaultMap.contains(key))
        return d->defaultMap[key];
    return d->defaultValue;
}

template <class T>
bool WMetaData<T>::setDefault(const QString &key, const T &value)
{
    d->defaultMap[key] = value;
    return !d->map.contains(key);
}

template <class T>
bool WMetaData<T>::hasArg(const QString &key) const
{
    return d->map.contains(key) || d->defaultMap.contains(key);
}

template <class T>
bool WMetaData<T>::set(const QString &key, const T &value)
{
    const bool existed = hasArg(key);
    d->map.insert(key, value);
    return existed;
}

template <class T>
void WMetaData<T>::setDefaultValue(const T &value)
{
    d->defaultValue = value;
}

template <class T>
QMap<QString, T> WMetaData<T>::toMap() const
{
    return d->map;
}

template <class T>
QMap<QString, T> WMetaData<T>::toMapDefault() const
{
    return d->defaultMap;
}

} // namespace we

#endif // WMETADATA_H