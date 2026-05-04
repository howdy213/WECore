/**
 * @file we.h
 * @brief Global singleton providing access to the application base instance.
 *
 * The WE class holds a static pointer to a WEBase instance and offers
 * static methods to initialise and retrieve it. It is a pure static
 * class and cannot be instantiated.
 *
 * @note This header is considered **ABI‑stable** and will **never** be
 *       modified in a way that breaks binary compatibility. Do not
 *       add, remove, or reorder virtual functions, members, or change
 *       the class layout.
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
#ifndef WE_H
#define WE_H

#include <QObject>

#include "WECore/Def/wedef.h"

namespace we {

/**
 * @brief Global static holder for the application’s WEBase instance.
 *
 * Use WE::init() once at startup to set the global instance; afterwards
 * any code can call WE::inst() to obtain it.
 */
class WE_EXPORT WE
{
    Q_DISABLE_COPY(WE)

public:
    /**
     * @brief Initialises the global WEBase instance.
     * @param base Pointer to the fully constructed WEBase singleton.
     *
     * Must be called exactly once before any calls to inst().
     */
    static bool init(WEBase *base);

    /**
     * @brief Returns the global WEBase instance.
     * @return Pointer to the WEBase, or @c nullptr if not yet initialised.
     */
    static WEBase *inst();

private:
    WE() = default;
    ~WE() = default;
    static WEBase *m_instance;
};

} // namespace we

#endif // WE_H