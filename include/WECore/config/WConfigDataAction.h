#ifndef WCONFIGDATAACTION_H
#define WCONFIGDATAACTION_H

#include "WConfigDataBase.h"
#include "WConfigDef.h"

namespace we::config {

class WE_EXPORT WConfigDataAction : public WConfigDataBase {
public:
    WConfigDataAction();

    WConfigDataBase *init(const QString& key, const WConfigItemInfo& info, WConfigViewer* parent = nullptr) override;
    WConfigDataAction* init(const QString& key, ActionCallback callback, const Properties& properties = {}, WConfigViewer* parent = nullptr);

    QVariant getTemporary() const override { return QVariant(); }
    bool setTemporary(const QVariant&) override { return false; }
    QVariant getPersistent() const override { return QVariant(); }
    bool setPersistent(const QVariant&, bool emitSignal = true) override { return true; }
    bool revertToPersistent() override { return false; }
    bool syncPersistent(bool emitSignal) override { return true; }
    QVariant toVariant() const override { return QVariant(); }
    bool fromVariant(const QVariant&) override { return false; }

    ActionCallback action() const { return info().callback(); }
    void execute() const;
};

} // namespace we::config

#endif // WCONFIGDATAACTION_H