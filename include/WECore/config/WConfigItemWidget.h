#ifndef WCONFIGITEMWIDGET_H
#define WCONFIGITEMWIDGET_H

#include "WConfigDataBase.h"
#include "WConfigEditorBase.h"
#include <QLabel>
#include <QToolButton>
#include <QVariant>
#include <QWidget>

namespace we::config {

class WE_EXPORT WConfigItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit WConfigItemWidget(WConfigDataBase *data, QWidget *parent = nullptr);

    WConfigDataBase *configData() const { return m_data; }
    QVariant currentValue() const;
    void setCurrentValue(const QVariant &value);
    void setSelected(bool selected);
signals:
    void clicked(we::config::WConfigItemWidget *widget);
    void valueChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onUndo();
    void onResetToDefault();

private:
    QColor m_defaultTextColor;
    WConfigEditorBase *createValueWidget();
    void updateUndoVisibility();
    QLabel *m_nameLabel = nullptr;
    QLabel *m_descLabel = nullptr;
    QToolButton *m_resetButton = nullptr;
    WConfigDataBase *m_data = nullptr;
    WConfigEditorBase *m_editor = nullptr;
    QToolButton *m_undoButton = nullptr;
    QSharedPointer<WConfigItemRef> m_itemRef;
};

} // namespace we::config

#endif // WCONFIGITEMWIDGET_H