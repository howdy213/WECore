/**
 * @file we.h
 * @brief WE类头文件，提供全局访问接口
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
#ifndef WE_H
#define WE_H
#include <QObject>

#include "../WDef/wedef.h"

W_INLINE namespace WE_NAMESPACE {
    class WE_EXPORT WE {
    public:
    static bool init(WEBase *base);
    static WEBase *inst();

private:
    WE() = default;
    ~WE() = default;
    WE(const WE &) = delete;
    WE &operator=(const WE &) = delete;
    static WEBase *m_instance;
    };
}
#endif
