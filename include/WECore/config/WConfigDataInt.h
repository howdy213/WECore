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
#ifndef WCONFIGDATAINT_H
#define WCONFIGDATAINT_H

#include "WConfigDataBase.h"

namespace we::config {

class WE_EXPORT WConfigDataInt : public WConfigDataBase {
public:
    WConfigDataInt();

    WConfigDataBase *init(const QString &key, const WConfigItemInfo &info,
                          WConfigViewer *parent = nullptr) override;
    WConfigDataInt *init(const QString &key, int defaultValue,
                         const Properties &properties = {},
                         WConfigViewer *parent = nullptr);

    QVariant getTemporary() const override { return m_value; }
    bool setTemporary(int value);
    bool setTemporary(const QVariant &value) override;
    QVariant getPersistent() const override { return m_original; }
    bool setPersistent(const QVariant &val, bool emitSignal = true) override;
    QVariant toVariant() const override;
    bool fromVariant(const QVariant &variant) override;

private:
    int m_value = 0;
    int m_original = 0;
};

} // namespace we::config

#endif // WCONFIGDATAINT_H