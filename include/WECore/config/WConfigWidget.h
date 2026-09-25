/**
 * @author howdy213
 * @date 2026-09-25
 * @version 2.1.0
 *
 * Copyright 2025-2026 howdy213
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef WCONFIGWIDGET_H
#define WCONFIGWIDGET_H

#include <QDialog>
#include <QTreeWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QHash>
#include <QShowEvent>
#include "WConfig.h"
#include "WConfigGroupWidget.h"
#include "WConfigItemWidget.h"

namespace we::config {

// Content-area layout modes: vertical (default), or horizontal / grid / form.
enum class WConfigContentLayout { Vertical, Horizontal, Grid, Form };

// Item-level layout adjuster. Called by the framework right after it creates an
// item's ItemWidget (group members included) so callers can tweak that widget
// (visibility, layout, embedding). The ItemWidget owns the data binding; the
// callback only adjusts appearance / layout.
using WConfigItemAdjuster =
    std::function<void(we::config::WConfigItemWidget *item)>;

// Full group-level custom layout: there is no runtime adjuster; supply it at
// group-creation time via WConfigGroupSpec::layoutBuilder (see WConfigViewer.h).

// Settings dialog: shows the config tree and renders the selected viewer's items as
// editors, with Save / Cancel and optional add / remove of items.
class WE_EXPORT WConfigWidget : public QDialog
{
    Q_OBJECT
public:
    explicit WConfigWidget(WConfig* config, QWidget* parent = nullptr);
    ~WConfigWidget();
    void addVerification(std::function<bool(WConfig*)> func);
    // Caller-driven refresh: re-read all visible editors in the current view from data.
    void refreshDisplay();
    // Plugin hook: append a merged group to the current viewer (applied immediately).
    void addConfigGroup(WConfigGroupSpec spec);
    // Switch content layout mode: reflow now if visible, otherwise defer to showEvent.
    void setContentLayout(WConfigContentLayout mode);
    // Inject a fully custom layout (ownership transferred to WConfigWidget); same deferral.
    void setContentLayout(QLayout* layout);
    // Register an item-level adjuster for key under the given viewer ("" = root).
    // Applied right after the ItemWidget is created, group members included.
    void setItemAdjuster(const QString& viewerPath, const QString& key,
                         WConfigItemAdjuster adjuster);
    // Path-less overload: applies to the root page (empty viewer path).
    void setItemAdjuster(const QString& key, WConfigItemAdjuster adjuster);
protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onTreeItemClicked(QTreeWidgetItem* item, int column);
    void onSaveClicked();
    void onCancelClicked();
    void onAddItemClicked();
    void onRemoveItemClicked();
    void onConfigChanged(we::config::WConfigDataBase* data);

private:
    WConfig* m_config;
    QTreeWidget* m_treeWidget;
    QScrollArea* m_scrollArea;
    QTreeWidgetItem *m_rootItem = nullptr;
    QWidget* m_contentWidget;
    QLayout* m_contentLayout = nullptr;
    QLabel* m_restartLabel;
    QPushButton* m_cancelButton;
    QPushButton* m_saveButton;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;

    WConfigViewer* m_currentViewer = nullptr;
    WConfigItemWidget* m_selectedItemWidget = nullptr;
    QList<WConfigItemWidget*> m_itemWidgets;             // Standalone item widgets
    QList<WConfigGroupWidget*> m_groupWidgets;           // Merged-group widgets
    QList<std::function<bool(WConfig*)>> m_verifications;
    WConfigContentLayout m_contentMode = WConfigContentLayout::Vertical;
    // Item adjusters keyed by "viewerPath/configKey".
    QHash<QString, WConfigItemAdjuster> m_itemAdjusters;
    QLayout* m_customLayout = nullptr;   // Fully custom layout (owned)
    bool m_pendingLayoutApply = false;   // Layout change pending until show
    // Root-viewer lock/unlock, called only from the constructor, destructor and save.
    // m_lockHeld tracks whether this instance holds the lock: it prevents re-locking
    // (which would accumulate the count) and ensures we only release the lock we took
    // (the root's lock may be held by a third party, so isLocked() must not decide).
    void acquireLock();
    void releaseLock();
    bool m_lockHeld = false;
    int m_gridRow = 0;                   // Current insertion row in grid/form mode
    void clearContentWidgets();
    void clearContentLayout();
    void rebuildContentLayout();
    void applyPendingLayout();
    // Build the "viewerPath/key" adjuster lookup key.
    QString adjusterKey(const QString& key) const;
    void applyItemAdjuster(WConfigItemWidget* w, const QString& key);
    WConfigItemWidget* findItemWidget(WConfigDataBase* data);
    void selectItemWidget(WConfigItemWidget* w);
    void addContentWidget(QWidget* w);
    void makeGroupWidget(WConfigViewer* viewer, WConfigGroupSpec* g);
    void makeItemWidget(WConfigViewer* viewer, WConfigDataBase* data);
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