#ifndef WCONFIGEDITORSTRING_H
#define WCONFIGEDITORSTRING_H

#include "WConfigDataString.h"
#include "WConfigEditorBase.h"
#include <QLineEdit>

namespace we::config {

class WE_EXPORT WConfigEditorString : public WConfigEditorBase
{
public:
    WConfigEditorString(WConfigItemWidget* parent = nullptr);
    void createEditor() override;
    void setConfigData(WConfigDataBase* data) override;
    WConfigDataString* getData();
    WConfigDataBase* configData() override;

private:
    QLineEdit* m_stringEdit = nullptr;
};

} // namespace we::config

#endif // WCONFIGEDITORSTRING_H