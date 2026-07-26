#ifndef WCONFIGEDITORACTION_H
#define WCONFIGEDITORACTION_H

#include "WConfigEditorBase.h"
#include <QPushButton>

namespace we::config {

class WE_EXPORT WConfigEditorAction : public WConfigEditorBase
{
    Q_OBJECT
public:
    WConfigEditorAction(WConfigItemWidget* parent = nullptr);
    void createEditor() override;
    void setConfigData(WConfigDataBase* data) override;
    WConfigDataBase* configData() override;

private slots:
    void onButtonClicked();

private:
    QPushButton* m_button = nullptr;
    WConfigDataAction* m_actionData = nullptr;
};

} // namespace we::config

#endif // WCONFIGEDITORACTION_H