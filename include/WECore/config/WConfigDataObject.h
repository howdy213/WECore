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
#ifndef WCONFIGDATAOBJECT_H
#define WCONFIGDATAOBJECT_H

#include "WConfigDataBase.h"
#include "WConfigViewer.h"
#include <QVariantMap>

namespace we::config {

enum class ObjectEditMode { ReadOnly, ModifyOnly, FullControl };

class WE_EXPORT WConfigDataObject : public WConfigDataBase {
public:
    WConfigDataObject();

    WConfigDataBase *init(const QString &key, const WConfigItemInfo &info,
                          WConfigViewer *parent = nullptr) override;
    WConfigDataObject *init(const QString &key,
                            const QVariantMap &defaultMap = {},
                            const Properties &properties = {},
                            WConfigViewer *parent = nullptr);

    ~WConfigDataObject() override;

    QVariant getTemporary() const override { return toVariant(); }
    bool setTemporary(const QVariant &value) override;
    QVariant getPersistent() const override { return m_originalMap; }
    bool setPersistent(const QVariant &val, bool emitSignal = true) override;
    bool revertToPersistent() override;
    QVariant toVariant() const override;
    bool fromVariant(const QVariant &variant) override;
    void buildDefaultMap();

    void forEachChild(std::function<void(WConfigDataBase *)> func) const override;
    void syncPersistentRecursive(bool emitSignal = true) override;
    void revertToPersistentRecursive() override;

    WConfigViewer *content() const { return m_content; }
    // return true if data is used
    bool addChildData(WConfigDataBase *data, bool isFromTemplate = false);
    WConfigDataBase *findChildData(const QString &key) const;

    ObjectEditMode editMode() const { return m_editMode; }
    void setEditMode(ObjectEditMode mode) { m_editMode = mode; }

    DeletionPolicy deletionPolicy() const { return m_deletionPolicy; }
    void setDeletionPolicy(DeletionPolicy policy) { m_deletionPolicy = policy; }
private:
    QVariantMap buildDefaultMapFromContent() const;
private:
    WConfigViewer *m_content = nullptr;
    QVariantMap m_originalMap;
    ObjectEditMode m_editMode = ObjectEditMode::FullControl;
    DeletionPolicy m_deletionPolicy = DeletionPolicy::AllowNonTemplateOnly;
};

} // namespace we::config

#endif // WCONFIGDATAOBJECT_H