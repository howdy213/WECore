/**
 * @file we.h
 * @brief Global singleton providing access to the application base instance.
 *
 * WE holds a static pointer to a WEBase instance and exposes static methods
 * to install and retrieve it. It is a pure static class and cannot be
 * instantiated.
 *
 * @note This header is **ABI-stable**: do not add, remove, or reorder data
 *       members or virtual functions, or otherwise change the class layout.
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
#ifndef WE_H
#define WE_H

#include <QObject>

#include "WECore/def/wedef.h"

namespace we {

/**
 * @brief Global holder for the application's single WEBase instance.
 *
 * Call init() once at startup to install the instance; afterwards any code
 * can call inst() to obtain it.
 */
class WE_EXPORT WE
{
    Q_DISABLE_COPY(WE)

public:
    /**
     * @brief Installs @p base as the global instance.
     *
     * Must be called exactly once, before any call to inst().
     * @return @c true on success; @c false if @p base is null or an
     *         instance has already been installed.
     */
    static bool init(WEBase *base);

    /// Returns the global instance, or @c nullptr before init() succeeds.
    static WEBase *inst();

private:
    WE() = default;
    ~WE() = default;
    static WEBase *m_instance;
};

} // namespace we

#endif // WE_H