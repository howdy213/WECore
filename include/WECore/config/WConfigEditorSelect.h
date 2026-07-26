#ifndef WCONFIGEDITORSELECT_H
#define WCONFIGEDITORSELECT_H

#include "WConfigEditorBase.h"
#include "WConfigDataSelect.h"
#include <QComboBox>

namespace we::config {

class WE_EXPORT WConfigEditorSelect : public WConfigEditorBase
{
public:
    WConfigEditorSelect(WConfigItemWidget* parent = nullptr);
    void createEditor() override;
    void setConfigData(WConfigDataBase* data) override;
    WConfigDataSelect* getData();
    WConfigDataBase* configData() override;

private:
    QComboBox* m_combo = nullptr;
};

} // namespace we::config

#endif // WCONFIGEDITORSELECT_H