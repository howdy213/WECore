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
#ifndef WCONFIGDATACUSTOM_H
#define WCONFIGDATACUSTOM_H

#include "WConfigDataBase.h"

namespace we::config {

class WE_EXPORT WConfigDataCustom : public WConfigDataBase {
public:
    WConfigDataCustom();

    WConfigDataBase *init(const QString &key, const WConfigItemInfo &info,
                          WConfigViewer *parent = nullptr) override;
    WConfigDataCustom *init(const QString &key, const QString &typeName,
                            const QVariant &defaultValue = {},
                            const Properties &properties = {},
                            WConfigViewer *parent = nullptr);

    QString typeName() const { return m_typeName; }

    // The temporary value is the UI value (m_value); the persistent value is the serialized form (m_saved)
    QVariant getTemporary() const override { return m_value; }
    bool setTemporary(const QVariant &value) override;
    QVariant getPersistent() const override { return m_saved; }
    bool setPersistent(const QVariant &val, bool emitSignal = true) override;
    bool modified() const override;
    QVariant toVariant() const override;   // = serialize(m_value)
    bool fromVariant(const QVariant &variant) override; // storage -> deserialize -> m_value

private:
    QString m_typeName;
    QVariant m_value;
    QVariant m_saved;
};

} // namespace we::config

#endif // WCONFIGDATACUSTOM_H