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
#ifndef WCONFIGDATASELECT_H
#define WCONFIGDATASELECT_H

#include "WConfigDataBase.h"
#include <QStringList>

namespace we::config {

class WE_EXPORT WConfigDataSelect : public WConfigDataBase
{
public:
    WConfigDataSelect();

    WConfigDataBase* init(const QString& key, const WConfigItemInfo& info, WConfigViewer* parent = nullptr) override;
    WConfigDataSelect* init(const QString& key, const QString& defaultValue, const QStringList& options,
              const Properties& properties = {}, WConfigViewer* parent = nullptr);

    QVariant getTemporary() const override { return m_value; }
    bool setTemporary(const QString& value);
    bool setTemporary(const QVariant& value) override;
    QVariant getPersistent() const override { return m_original; }
    bool setPersistent(const QVariant& val, bool emitSignal = true) override;
    QVariant toVariant() const override;
    bool fromVariant(const QVariant& variant) override;

    QStringList options() const { return m_info.options(); }
    void addOption(const QString& option);
    void addOptions(const QStringList& options);

private:
    QString m_value;
    QString m_original;
};

} // namespace we::config

#endif // WCONFIGDATASELECT_H