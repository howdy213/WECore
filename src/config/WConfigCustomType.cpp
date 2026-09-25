/**
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
#include "WECore/config/WConfigCustomType.h"
#include "WECore/config/WConfigCustomBuiltins.h"
#include <QHash>

namespace we::config {

class WConfigCustomTypeRegistry::Impl {
public:
    bool builtinsRegistered = false;
    QHash<QString, WConfigCustomType> types;
};

WConfigCustomTypeRegistry::WConfigCustomTypeRegistry() : m_impl(new Impl) {
}

WConfigCustomTypeRegistry::~WConfigCustomTypeRegistry() { delete m_impl; }

WConfigCustomTypeRegistry &WConfigCustomTypeRegistry::instance() {
    // The static local instance guarantees thread-safe first construction; registerBuiltins()
    // -> registerBuiltinCustomTypes() calls back into this function, and re-entry is guarded by
    // Impl::builtinsRegistered, so here we only need to set the static guard after registration completes.
    static WConfigCustomTypeRegistry inst;
    if(!buildinRegistered){
        inst.registerBuiltins();
        buildinRegistered=true;
    }
    return inst;
}

bool WConfigCustomTypeRegistry::registerType(const WConfigCustomType &spec) {
    if (spec.typeName.isEmpty() || !spec.serialize || !spec.deserialize)
        return false;
    if (m_impl->types.contains(spec.typeName))
        return false;
    m_impl->types.insert(spec.typeName, spec);
    return true;
}

bool WConfigCustomTypeRegistry::unregisterType(const QString &typeName) {
    return m_impl->types.remove(typeName) > 0;
}

const WConfigCustomType *WConfigCustomTypeRegistry::type(
    const QString &typeName) const {
    auto it = m_impl->types.constFind(typeName);
    return it == m_impl->types.constEnd() ? nullptr : &it.value();
}

bool WConfigCustomTypeRegistry::hasType(const QString &typeName) const {
    return m_impl->types.contains(typeName);
}

QStringList WConfigCustomTypeRegistry::typeNames() const {
    return m_impl->types.keys();
}

void WConfigCustomTypeRegistry::registerBuiltins() {
    if (m_impl->builtinsRegistered)
        return;
    m_impl->builtinsRegistered = true;
    registerBuiltinCustomTypes();
}

bool WConfigCustomTypeRegistry::buildinRegistered=false;

} // namespace we::config