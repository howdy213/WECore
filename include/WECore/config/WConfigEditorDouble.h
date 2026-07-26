#ifndef WCONFIGEDITORDOUBLE_H
#define WCONFIGEDITORDOUBLE_H

#include "WConfigDataDouble.h"
#include "WConfigEditorBase.h"
#include <QDoubleSpinBox>

namespace we::config {

class WE_EXPORT WConfigEditorDouble : public WConfigEditorBase
{
public:
    WConfigEditorDouble(WConfigItemWidget* parent = nullptr);
    void createEditor() override;
    void setConfigData(WConfigDataBase* data) override;
    WConfigDataDouble* getData();
    WConfigDataBase* configData() override;

private:
    QDoubleSpinBox* m_doubleSpin = nullptr;
};

} // namespace we::config

#endif // WCONFIGEDITORDOUBLE_H