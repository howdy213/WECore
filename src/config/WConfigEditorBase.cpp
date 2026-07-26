#include "WECore/config/WConfigEditorBase.h"
#include "WECore/config/WConfigItemWidget.h"

namespace we::config {

WConfigEditorBase::WConfigEditorBase(WConfigItemWidget* parent) : QWidget(parent) {}

void WConfigEditorBase::setConfigValue(QVariant value) { if (m_data) m_data->setTemporary(value); }
void WConfigEditorBase::setConfigData(WConfigDataBase* data) { if (data && data->type() == m_type) m_data = data; }
WConfigDataBase* WConfigEditorBase::configData() { return m_data; }
void WConfigEditorBase::createEditor() {}

QWidget *NoEditColumnDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.column() == 0)
        return nullptr;
    return QStyledItemDelegate::createEditor(parent, option, index);
}

} // namespace we::config