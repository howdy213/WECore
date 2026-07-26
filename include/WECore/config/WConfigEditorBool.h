#ifndef WCONFIGEDITORBOOL_H
#define WCONFIGEDITORBOOL_H

#include "WConfigDataBool.h"
#include "WConfigEditorBase.h"
#include <QCheckBox>

namespace we::config {

class WE_EXPORT WConfigEditorBool : public WConfigEditorBase
{
public:
    WConfigEditorBool(WConfigItemWidget* parent = nullptr);
    void createEditor() override;
    void setConfigData(WConfigDataBase* data) override;
    WConfigDataBool* getData();
    WConfigDataBase* configData() override;

private:
    QCheckBox* m_boolCheck = nullptr;
};

} // namespace we::config

#endif // WCONFIGEDITORBOOL_H