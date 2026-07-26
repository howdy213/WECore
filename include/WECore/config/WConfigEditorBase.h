#ifndef WCONFIGEDITORBASE_H
#define WCONFIGEDITORBASE_H

#include "WConfigDataBase.h"
#include <QStyledItemDelegate>
#include <QWidget>

namespace we::config {

class NoEditColumnDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
};

class WConfigItemWidget;

class WE_EXPORT WConfigEditorBase : public QWidget {
    Q_OBJECT
public:
    WConfigEditorBase(WConfigItemWidget *parent = nullptr);

    virtual void setConfigValue(QVariant value);
    virtual void setConfigData(WConfigDataBase *data);
    virtual WConfigDataBase *configData();
    virtual void createEditor();

signals:
    void valueChanged();  // 当编辑器值被用户修改时发射
protected:
    WConfigDataBase *m_data = nullptr;
    DataType m_type = DataType::None;
};

} // namespace we::config

#endif // WCONFIGEDITORBASE_H