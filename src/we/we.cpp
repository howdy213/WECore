/**
 * @file we.cpp
 * @brief Implementation of the WE global instance holder.
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
#include "WECore/we/we.h"
#include "WECore/we/webase.h"
namespace we {
WEBase *WE::m_instance = nullptr;

bool WE::init(WEBase *base) {
    if (!base) {
        return false;
    }
    // The application owns exactly one WEBase. Re-initialisation is rejected
    // so that inst() never changes underneath existing users.
    if (m_instance != nullptr) {
        return false;
    }
    m_instance = base;
    return true;
}

WEBase *WE::inst() { return m_instance; }
} // namespace we