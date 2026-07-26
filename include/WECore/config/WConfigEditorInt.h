#ifndef WCONFIGEDITORInt_H
#define WCONFIGEDITORInt_H

#include "WConfigDataInt.h"
#include "WConfigEditorBase.h"
#include <QSpinBox>

namespace we::config {

class WE_EXPORT WConfigEditorInt : public WConfigEditorBase
{
public:
    WConfigEditorInt(WConfigItemWidget* parent = nullptr);
    void createEditor() override;
    void setConfigData(WConfigDataBase* data) override;
    WConfigDataInt* getData();
    WConfigDataBase* configData() override;

private:
    QSpinBox* m_intSpin = nullptr;
};

} // namespace we::config

#endif // WCONFIGEDITORInt_H