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
#include "WECore/config/WConfigDataAction.h"

namespace we::config {

WConfigDataAction::WConfigDataAction() { m_type = DataType::Action; }

WConfigDataBase *WConfigDataAction::init(const QString &key,
                                         const WConfigItemInfo &info,
                                         WConfigViewer *parent) {
    WConfigDataBase::init(DataType::Action, key, info, parent);
    return this;
}

WConfigDataAction *WConfigDataAction::init(const QString &key,
                                           ActionCallback callback,
                                           const Properties &properties,
                                           WConfigViewer *parent) {
    WConfigItemInfo info;
    info.callback(callback);
    for (Property p : properties)
        info.property(p);
    init(key, info, parent);
    return this;
}

void WConfigDataAction::execute() const { info().callback()(); }

} // namespace we::config