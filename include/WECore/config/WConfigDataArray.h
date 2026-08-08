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
#ifndef WCONFIGDATAARRAY_H
#define WCONFIGDATAARRAY_H

#include "WConfigDataBase.h"
#include <QVariantList>

namespace we::config {

class WE_EXPORT WConfigDataArray : public WConfigDataBase {
public:
    WConfigDataArray();

    WConfigDataBase *init(const QString &key, const WConfigItemInfo &info,
                          WConfigViewer *parent = nullptr) override;
    WConfigDataArray *init(const QString &key, const QVariantList &defaultValue,
                           DataType elementType,
                           const Properties &properties = {},
                           WConfigViewer *parent = nullptr);

    QVariant getTemporary() const override { return m_value; }
    bool setTemporary(const QVariant &value) override;
    QVariant getPersistent() const override { return m_originalList; }
    bool setPersistent(const QVariant &val, bool emitSignal = true) override;
    QVariant toVariant() const override { return m_value; }
    bool fromVariant(const QVariant &variant) override;

    DataType elementType() const { return m_info.elementType(); }
    int count() const { return m_value.count(); }
    QVariant elementAt(int index) const;
    bool addElement(const QVariant &element);
    bool setElement(int index, const QVariant &element);
    bool insertElement(int index, const QVariant &element);
    bool removeElement(int index);

private:
    QVariantList m_value;
    QVariantList m_originalList;
};

} // namespace we::config

#endif // WCONFIGDATAARRAY_H