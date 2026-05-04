/**
 * @file wmetadata.h
 * @brief Template class for key‑value metadata storage with defaults.
 *
 * WMetaData provides a dictionary‑like container that supports per‑key
 * defaults, a global fallback value, and import/export via QMap.
 *
 * @author howdy213
 * @date 2026-05-01
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
#ifndef WMETADATA_H
#define WMETADATA_H

#include <QMap>
#include <QScopedPointer>
#include <QString>

#include "WECore/Def/wedef.h"

namespace we {

/**
 * @brief Private data for the WMetaData template (d‑pointer pattern).
 *
 * Stores the actual user‑set values, per‑key fallback defaults,
 * and a global default value.
 */
template <class T>
class WConfigPrivate
{
public:
    QMap<QString, T> map;         ///< User‑set key‑value pairs.
    QMap<QString, T> defaultMap;  ///< Per‑key fallback values.
    T defaultValue;               ///< Global fallback value.
};

/**
 * @brief A key‑value metadata container with hierarchical defaults.
 *
 * Values are retrieved in the following order:
 * 1. User‑set value for the key.
 * 2. Per‑key default value.
 * 3. Global default value.
 *
 * The class is non‑copyable and uses the Qt d‑pointer to keep binary
 * compatibility.
 */
template <class T>
class WE_EXPORT WMetaData
{
    Q_DISABLE_COPY(WMetaData)

public:
    /// Constructs an empty metadata container.
    WMetaData();

    /// Destroys the container. The private data is automatically cleaned up.
    virtual ~WMetaData();

    /**
     * @brief Retrieves the value associated with @p key.
     * @param key The key to look up.
     * @return The stored value, or the appropriate default if not found.
     */
    T get(const QString &key) const;

    /**
     * @brief Sets a per‑key default value.
     * @param key   The key for which to set the default.
     * @param value The default value.
     * @return @c true if no user‑set value existed for the key (i.e., the
     *         default will be used for future calls to get()), @c false
     *         if a user value already overrides this default.
     */
    bool setDefault(const QString &key, const T &value);

    /**
     * @brief Checks whether a key has any value (user‑set or default).
     * @param key The key to check.
     * @return @c true if the key exists.
     */
    bool hasArg(const QString &key) const;

    /**
     * @brief Stores a user‑set value for @p key.
     * @param key   The key.
     * @param value The value to store.
     * @return @c true if the key previously existed (either user‑set or default),
     *         @c false if this is a new key.
     */
    bool set(const QString &key, const T &value);

    /**
     * @brief Sets the global fallback default value.
     * @param value The global default.
     *
     * This value is returned by get() when no user‑set or per‑key default exists.
     */
    void setDefaultValue(const T &value);

    /**
     * @brief Returns a copy of all user‑set key‑value pairs.
     */
    QMap<QString, T> toMap() const;

    /**
     * @brief Returns a copy of all per‑key default values.
     */
    QMap<QString, T> toMapDefault() const;

protected:
    QScopedPointer<WConfigPrivate<T>> d; ///< Private data object.
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