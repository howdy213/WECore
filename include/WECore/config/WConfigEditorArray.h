#ifndef WCONFIGEDITORARRAY_H
#define WCONFIGEDITORARRAY_H

#include "WConfigDataArray.h"
#include "WConfigEditorBase.h"
#include <QPushButton>
#include <QTreeWidget>

namespace we::config {

class WE_EXPORT WConfigEditorArray : public WConfigEditorBase {
public:
    WConfigEditorArray(WConfigItemWidget *parent = nullptr);
    void createEditor() override;
    void setConfigData(WConfigDataBase *data) override;
    WConfigDataArray *getData();
    WConfigDataBase *configData() override;

private slots:
    void onAddArrayElement();
    void onRemoveArrayElement();
    void onInsertBefore();
    void onInsertAfter();
    void onArrayItemChanged(QTreeWidgetItem *item, int column);
    void insertElementAt(int position);

private:
    void refreshTree();
    QTreeWidget *m_arrayTree = nullptr;
    QPushButton *m_addArrayBtn = nullptr;
    QPushButton *m_removeArrayBtn = nullptr;
    QPushButton *m_insertAfterBtn = nullptr;
    QPushButton *m_insertBeforeBtn = nullptr;
};

} // namespace we::config

#endif // WCONFIGEDITORARRAY_H