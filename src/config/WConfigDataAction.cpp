#include "WECore/config/WConfigDataAction.h"

namespace we::config {

WConfigDataAction::WConfigDataAction() { m_type = DataType::Action; }

WConfigDataBase* WConfigDataAction::init(const QString& key, const WConfigItemInfo& info, WConfigViewer* parent)
{
    WConfigDataBase::init(DataType::Action, key, info, parent);
    return this;
}

WConfigDataAction* WConfigDataAction::init(const QString& key, ActionCallback callback, const Properties& properties, WConfigViewer* parent)
{
    WConfigItemInfo info;
    info.callback(callback);
    for (Property p : properties) info.property(p);
    init(key, info, parent);
    return this;
}

void WConfigDataAction::execute() const { info().callback(); }

} // namespace we::config