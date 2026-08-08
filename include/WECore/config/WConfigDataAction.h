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
#ifndef WCONFIGDATAACTION_H
#define WCONFIGDATAACTION_H

#include "WConfigDataBase.h"
#include "WConfigDef.h"

namespace we::config {

class WE_EXPORT WConfigDataAction : public WConfigDataBase {
public:
    WConfigDataAction();

    WConfigDataBase *init(const QString &key, const WConfigItemInfo &info,
                          WConfigViewer *parent = nullptr) override;
    WConfigDataAction *init(const QString &key, ActionCallback callback,
                            const Properties &properties = {},
                            WConfigViewer *parent = nullptr);

    QVariant getTemporary() const override { return QVariant(); }
    bool setTemporary(const QVariant &) override { return false; }
    QVariant getPersistent() const override { return QVariant(); }
    bool setPersistent(const QVariant &, bool emitSignal = true) override {
        Q_UNUSED(emitSignal);
        return true;
    }
    bool revertToPersistent() override { return false; }
    bool syncPersistent(bool emitSignal) override {
        return true;
        Q_UNUSED(emitSignal);
    }
    QVariant toVariant() const override { return QVariant(); }
    bool fromVariant(const QVariant &) override { return false; }

    ActionCallback action() const { return info().callback(); }
    void execute() const;
};

} // namespace we::config

#endif // WCONFIGDATAACTION_H