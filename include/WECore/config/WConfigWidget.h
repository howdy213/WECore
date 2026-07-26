#ifndef WCONFIGWIDGET_H
#define WCONFIGWIDGET_H

#include <QDialog>
#include <QTreeWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include "WConfig.h"
#include "WConfigItemWidget.h"

namespace we::config {

class WE_EXPORT WConfigWidget : public QDialog
{
    Q_OBJECT
public:
    explicit WConfigWidget(WConfig* config, QWidget* parent = nullptr);
    ~WConfigWidget();
    void addVerification(std::function<bool(WConfig*)> func);

private slots:
    void onTreeItemClicked(QTreeWidgetItem* item, int column);
    void onSaveClicked();
    void onCancelClicked();
    void onAddItemClicked();
    void onRemoveItemClicked();

private:
    WConfig* m_config;
    QTreeWidget* m_treeWidget;
    QScrollArea* m_scrollArea;
    QTreeWidgetItem *m_rootItem = nullptr;
    QWidget* m_contentWidget;
    QVBoxLayout* m_contentLayout;
    QLabel* m_restartLabel;
    QPushButton* m_cancelButton;
    QPushButton* m_saveButton;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;

    WConfigViewer* m_currentViewer = nullptr;
    WConfigItemWidget* m_selectedItemWidget = nullptr;
    QList<WConfigItemWidget*> m_itemWidgets;
    QList<std::function<bool(WConfig*)>> m_verifications;
    void syncCurrentPage();
    void buildTree(WConfigViewer* viewer, QTreeWidgetItem* parentItem = nullptr);
    void displayConfig(WConfigViewer* viewer);
    void refreshCurrentPage();
    void setupAcceptPolicyButtons(WConfigViewer* viewer);
    void updateRestartPrompt();
    void forEachConfigData(std::function<void(WConfigDataBase*)> func);
};

} // namespace we::config

#endif // WCONFIGWIDGET_H