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
#include "WECore/config/WConfigWidget.h"
#include "WECore/config/WConfigDataDef.h"
#include "WECore/config/WConfigLayout.h"
#include <QApplication>
#include <QBoxLayout>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QSet>
#include <QSplitter>
#include <QVBoxLayout>
#include <functional>

namespace we::config {

WConfigWidget::WConfigWidget(WConfig *config, QWidget *parent)
    : QDialog(parent), m_config(config), m_treeWidget(new QTreeWidget(this)),
    m_scrollArea(new QScrollArea(this)),
    m_contentWidget(new QWidget(m_scrollArea)),
    m_contentLayout(new QVBoxLayout(m_contentWidget)),
    m_restartLabel(new QLabel(
          tr("Some settings will take effect after restart."), this)),
    m_cancelButton(new QPushButton(tr("Cancel"), this)),
    m_saveButton(new QPushButton(tr("Save"), this)),
    m_addButton(new QPushButton(tr("Add Item"), this)),
    m_removeButton(new QPushButton(tr("Remove Selected"), this)) {
    setWindowTitle(tr("Settings"));
    setMinimumSize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(Layout::DialogMargin, Layout::DialogMargin,
                                   Layout::DialogMargin, Layout::DialogMargin);
    mainLayout->setSpacing(Layout::RowSpacing);
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setMinimumWidth(200);
    splitter->addWidget(m_treeWidget);

    m_rootItem = new QTreeWidgetItem();
    WConfigViewer *rootViewer = m_config->document()->root();
    QString rootDisplay = rootViewer->displayName();
    if (rootDisplay.isEmpty())
        rootDisplay = tr("Root");
    m_rootItem->setText(0, rootDisplay);
    m_rootItem->setToolTip(0, rootViewer->description());
    m_rootItem->setData(0, Qt::UserRole, ""); // Empty path denotes the root
    m_treeWidget->addTopLevelItem(m_rootItem);
    m_treeWidget->expandItem(m_rootItem);
    m_treeWidget->setRootIsDecorated(false);

    // Keep the root node always expanded.
    connect(m_treeWidget, &QTreeWidget::itemCollapsed, this,
            [this](QTreeWidgetItem *item) {
                if (item == m_rootItem)
                    m_treeWidget->expandItem(item);
            });

    buildTree(rootViewer, m_rootItem);

    m_treeWidget->setCurrentItem(m_rootItem);
    onTreeItemClicked(m_rootItem, 0);

    connect(m_treeWidget, &QTreeWidget::itemClicked, this,
            &WConfigWidget::onTreeItemClicked);

    m_scrollArea->setWidgetResizable(true);
    m_contentLayout->setAlignment(Qt::AlignTop);
    m_contentLayout->setSpacing(Layout::PageSpacing);
    // Slight left inset for the main area (overridable via setContentLayout).
    m_contentLayout->setContentsMargins(Layout::PageLeftMargin, 0,
                                       Layout::PageLeftMargin, 0);
    m_scrollArea->setWidget(m_contentWidget);
    splitter->addWidget(m_scrollArea);
    splitter->setSizes({200, 600});
    mainLayout->addWidget(splitter);

    // Refresh the affected item widget immediately on data changes (e.g. dynamic properties).
    connect(m_config, &WConfig::configChanged, this,
            &WConfigWidget::onConfigChanged);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    m_restartLabel->setStyleSheet("color: #e67e22;");
    m_restartLabel->setObjectName("RestartLabel");
    m_restartLabel->setVisible(false);
    bottomLayout->addWidget(m_restartLabel);
    bottomLayout->addStretch();

    m_addButton->setVisible(false);
    m_removeButton->setVisible(false);
    connect(m_addButton, &QPushButton::clicked, this,
            &WConfigWidget::onAddItemClicked);
    connect(m_removeButton, &QPushButton::clicked, this,
            &WConfigWidget::onRemoveItemClicked);
    bottomLayout->addWidget(m_addButton);
    bottomLayout->addWidget(m_removeButton);

    connect(m_cancelButton, &QPushButton::clicked, this,
            &WConfigWidget::onCancelClicked);
    bottomLayout->addWidget(m_cancelButton);
    m_saveButton->setDefault(true);
    connect(m_saveButton, &QPushButton::clicked, this,
            &WConfigWidget::onSaveClicked);
    bottomLayout->addWidget(m_saveButton);

    mainLayout->addLayout(bottomLayout);

    acquireLock();
    updateRestartPrompt();
}

WConfigWidget::~WConfigWidget() { releaseLock(); }

// Root-viewer lock/unlock, called only from the constructor, destructor and save.
// m_lockHeld records whether this instance holds the lock: re-locking is skipped
// (which would accumulate the count), and only the lock we took is released --
// isLocked() is not consulted since the root's lock may be held by a third party.
void WConfigWidget::acquireLock() {
    if (m_lockHeld || !m_config || !m_config->document() ||
        !m_config->document()->root())
        return;
    m_config->document()->root()->lock();
    m_lockHeld = true;
}

void WConfigWidget::releaseLock() {
    if (!m_lockHeld || !m_config || !m_config->document() ||
        !m_config->document()->root())
        return;
    m_config->document()->root()->unlock();
    m_lockHeld = false;
}

void WConfigWidget::addVerification(std::function<bool(WConfig *)> func) {
    if (func) {
        m_verifications.append(func);
    }
}

// Clear the content area's widgets and related bookkeeping to avoid dangling pointers.
// Only removes layout items (destroying their widgets); the layout itself is kept.
void WConfigWidget::clearContentWidgets() {
    if (m_contentLayout) {
        QLayoutItem *child;
        while ((child = m_contentLayout->takeAt(0)) != nullptr) {
            if (child->widget())
                delete child->widget();
            delete child;
        }
    }
    m_itemWidgets.clear();
    m_groupWidgets.clear();
    m_selectedItemWidget = nullptr;
    m_gridRow = 0;
}

void WConfigWidget::clearContentLayout() {
    if (!m_contentLayout)
        return;
    // Remove all children first (destroying their widgets), then delete the layout
    // so it uninstalls from m_contentWidget before a new one is set.
    clearContentWidgets();
    // m_customLayout may alias m_contentLayout; null it too, or the next
    // rebuildContentLayout would reinstall a freed layout (dangling pointer).
    if (m_customLayout == m_contentLayout)
        m_customLayout = nullptr;
    delete m_contentLayout;
    m_contentLayout = nullptr;
}

void WConfigWidget::rebuildContentLayout() {
    clearContentLayout();

    if (m_customLayout) {
        // Fully custom layout: install the caller-injected QLayout directly.
        m_contentLayout = m_customLayout;
        m_contentWidget->setLayout(m_customLayout);
        return;
    }

    switch (m_contentMode) {
    case WConfigContentLayout::Horizontal:
        m_contentLayout = new QHBoxLayout(m_contentWidget);
        break;
    case WConfigContentLayout::Grid:
        m_contentLayout = new QGridLayout(m_contentWidget);
        break;
    case WConfigContentLayout::Form:
        m_contentLayout = new QFormLayout(m_contentWidget);
        break;
    case WConfigContentLayout::Vertical:
    default:
        m_contentLayout = new QVBoxLayout(m_contentWidget);
        m_contentLayout->setContentsMargins(Layout::PageLeftMargin, 0,
                                           Layout::PageLeftMargin, 0);
        break;
    }
    m_contentLayout->setAlignment(Qt::AlignTop);
    m_contentLayout->setSpacing(Layout::PageSpacing);
}

// Single entry point for layout changes: reflow now if visible, otherwise mark it
// pending (applied in showEvent) so rebuilding never happens before show().
void WConfigWidget::applyPendingLayout() {
    if (isVisible() && m_treeWidget->currentItem()) {
        rebuildContentLayout();
        onTreeItemClicked(m_treeWidget->currentItem(), 0);
    } else {
        m_pendingLayoutApply = true;
    }
}

void WConfigWidget::showEvent(QShowEvent *event) {
    QDialog::showEvent(event);
    if (m_pendingLayoutApply) {
        m_pendingLayoutApply = false;
        if (m_treeWidget->currentItem()) {
            rebuildContentLayout();
            onTreeItemClicked(m_treeWidget->currentItem(), 0);
        }
    }
}

void WConfigWidget::setContentLayout(WConfigContentLayout mode) {
    // Layout mode is independent of item/group adjusters: switching must not clear them.
    if (m_customLayout || m_contentMode != mode) {
        m_customLayout = nullptr;
        m_contentMode = mode;
        applyPendingLayout();
    }
}

void WConfigWidget::setContentLayout(QLayout *layout) {
    if (!layout || m_customLayout == layout)
        return;
    m_customLayout = layout;
    m_contentMode = WConfigContentLayout::Vertical; // Actual layout determined by m_customLayout
    applyPendingLayout();
}

// Build the "viewerPath/key" adjuster lookup key (empty path means the root viewer).
QString WConfigWidget::adjusterKey(const QString &key) const {
    const QString viewerPath =
        m_currentViewer ? m_currentViewer->fullPath() : QString();
    return (viewerPath.isEmpty() ? QString() : viewerPath + "/") + key;
}

void WConfigWidget::setItemAdjuster(const QString &viewerPath,
                                    const QString &key,
                                    WConfigItemAdjuster adjuster) {
    QString mapKey = (viewerPath.isEmpty() ? QString() : viewerPath + "/") + key;
    if (adjuster)
        m_itemAdjusters.insert(mapKey, std::move(adjuster));
    else
        m_itemAdjusters.remove(mapKey);
    // Defer to show if hidden; otherwise rebuild the current page to apply the adjuster.
    applyPendingLayout();
}

void WConfigWidget::setItemAdjuster(const QString &key,
                                    WConfigItemAdjuster adjuster) {
    setItemAdjuster(QString(), key, std::move(adjuster));
}

// Apply an item adjuster; group members and standalone items share this lookup.
void WConfigWidget::applyItemAdjuster(WConfigItemWidget *w, const QString &key) {
    if (!w)
        return;
    auto it = m_itemAdjusters.constFind(adjusterKey(key));
    if (it != m_itemAdjusters.constEnd() && *it)
        (*it)(w);
}

// Add the next config item into the current content layout, choosing the position by mode.
void WConfigWidget::addContentWidget(QWidget *w) {
    if (!w || !m_contentLayout)
        return;
    if (auto *g = qobject_cast<QGridLayout *>(m_contentLayout)) {
        g->addWidget(w, m_gridRow++, 0); // Grid mode: one item per row, in order
        return;
    }
    if (auto *f = qobject_cast<QFormLayout *>(m_contentLayout)) {
        f->addRow(w); // Form mode: one row per item
        return;
    }
    m_contentLayout->addWidget(w); // Vertical / Horizontal / Custom: append in order
}

void WConfigWidget::selectItemWidget(WConfigItemWidget *w) {
    if (m_selectedItemWidget == w)
        return;
    for (auto *item : std::as_const(m_itemWidgets))
        item->setSelected(false);
    for (auto *group : std::as_const(m_groupWidgets)) {
        for (auto *item : group->memberWidgets())
            item->setSelected(false);
    }
    w->setSelected(true);
    m_selectedItemWidget = w;
}

// Locate the widget for a data item: standalone items first, then group members.
// Groups replace their member widgets on rebuild, so no long-lived cache is kept.
WConfigItemWidget *WConfigWidget::findItemWidget(WConfigDataBase *data) {
    if (!data)
        return nullptr;
    for (auto *item : std::as_const(m_itemWidgets)) {
        if (item->configData() == data)
            return item;
    }
    for (auto *group : std::as_const(m_groupWidgets)) {
        if (auto *item = group->memberWidget(data->key()))
            return item;
    }
    return nullptr;
}

// Create and place a merged-group widget and wire its signals.
void WConfigWidget::makeGroupWidget(WConfigViewer *viewer, WConfigGroupSpec *g) {
    auto *groupWidget = new WConfigGroupWidget(viewer, g, m_contentWidget);
    // Hook so group members get item adjusters applied on every (re)build.
    groupWidget->setItemHook([this](WConfigItemWidget *w, const QString &key) {
        applyItemAdjuster(w, key);
    });
    // The constructor already created one round of members; adjust those now.
    for (auto *w : groupWidget->memberWidgets()) {
        if (w->configData())
            applyItemAdjuster(w, w->configData()->key());
    }
    addContentWidget(groupWidget);
    m_groupWidgets.append(groupWidget);
    connect(groupWidget, &WConfigGroupWidget::clicked, this,
            [this](WConfigItemWidget *w) { selectItemWidget(w); });
    connect(groupWidget, &WConfigGroupWidget::valueChanged, this,
            &WConfigWidget::updateRestartPrompt);
}

// Create and place a config item widget and wire its signals.
// Standalone items (not in a merged group) keep the full style (title + description + editor).
void WConfigWidget::makeItemWidget(WConfigViewer *viewer, WConfigDataBase *data) {
    Q_UNUSED(viewer);
    auto *itemWidget = new WConfigItemWidget(data, WConfigItemWidgetStyle::Full,
                                             m_contentWidget);
    addContentWidget(itemWidget);
    m_itemWidgets.append(itemWidget);
    connect(itemWidget, &WConfigItemWidget::clicked, this,
            [this](WConfigItemWidget *w) { selectItemWidget(w); });
    connect(itemWidget, &WConfigItemWidget::valueChanged, this,
            &WConfigWidget::updateRestartPrompt);
    applyItemAdjuster(itemWidget, data->key());
}

void WConfigWidget::forEachConfigData(
    std::function<void(WConfigDataBase *)> func) {
    std::function<void(WConfigViewer *)> traverse = [&](WConfigViewer *viewer) {
        if (!viewer)
            return;
        for (WConfigDataBase *data : viewer->allConfigData()) {
            func(data);
            // Not recommended for Object internals; the library does not guarantee
            // that Object-internal settings remain functional.
            data->forEachChild(func);
        }
        for (WConfigViewer *child : viewer->children()) {
            traverse(child);
        }
    };
    traverse(m_config->document()->root());
}

void WConfigWidget::buildTree(WConfigViewer *viewer,
                              QTreeWidgetItem *parentItem) {
    for (WConfigViewer *child : viewer->children()) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        QString displayName =
            child->displayName().isEmpty() ? child->name() : child->displayName();
        item->setText(0, displayName);
        item->setToolTip(0, child->description());
        item->setData(0, Qt::UserRole, child->fullPath());
        if (parentItem)
            parentItem->addChild(item);
        else
            m_treeWidget->addTopLevelItem(item);
        if (!child->children().isEmpty())
            buildTree(child, item);
    }
}

void WConfigWidget::onTreeItemClicked(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column);
    syncCurrentPage();

    // Clear the content area's widgets and bookkeeping (single entry point; avoids dangling pointers).
    clearContentWidgets();

    QString path = item->data(0, Qt::UserRole).toString();
    WConfigViewer *viewer = nullptr;
    if (path.isEmpty()) {
        viewer = m_config->document()->root();
    } else {
        viewer = m_config->document()->root()->findChildViewer(path);
    }
    if (viewer)
        displayConfig(viewer);
    else
        updateRestartPrompt(); // Refresh button state even without a target view (avoids stale state)
}

void WConfigWidget::displayConfig(WConfigViewer *viewer) {
    m_currentViewer = viewer;

    // Default rendering: merged groups first, then ungrouped items in key order.
    // Fine-grained order/position/visibility is left to the item/group adjusters.
    for (WConfigGroupSpec *g : viewer->groups())
        makeGroupWidget(viewer, g);

    QSet<QString> groupedKeys;
    for (WConfigGroupSpec *g : viewer->groups()) {
        for (const auto &member : g->members)
            groupedKeys.insert(member.first);
    }
    for (WConfigDataBase *data : viewer->allConfigData()) {
        if (groupedKeys.contains(data->key()))
            continue;
        makeItemWidget(viewer, data);
    }
    // Only box layouts support addStretch (pushes content to the top/left); grid/form do not.
    if (auto *box = qobject_cast<QBoxLayout *>(m_contentLayout))
        box->addStretch();

    setupAcceptPolicyButtons(viewer);
    updateRestartPrompt();
}

void WConfigWidget::refreshCurrentPage() {
    if (m_currentViewer) {
        // Clear and re-display (single cleanup entry point; avoids dangling bookkeeping).
        clearContentWidgets();
        displayConfig(m_currentViewer);
    }
}

void WConfigWidget::setupAcceptPolicyButtons(WConfigViewer *viewer) {
    if (!viewer)
        return;
    bool accept = (viewer->effectiveAcceptPolicy() == AcceptPolicy::ACCEPT);
    m_addButton->setVisible(accept);
    m_removeButton->setVisible(accept);
}

void WConfigWidget::updateRestartPrompt() {
    bool anyRestartRequiredModified = false;
    forEachConfigData([&](WConfigDataBase *data) {
        if (data->hasProperty(Property::RestartRequired) && data->modified())
            anyRestartRequiredModified = true;
    });

    m_restartLabel->setVisible(anyRestartRequiredModified);
    if (anyRestartRequiredModified)
        m_restartLabel->setText(
            tr("Some settings require restart to take effect."));
    else
        m_restartLabel->setText(tr("No restart required."));
}

void WConfigWidget::onSaveClicked() {
    // Sync the current page's editors.
    setEnabled(false);
    syncCurrentPage();
    for (const auto &verify : m_verifications) {
        if (!verify(m_config)) {
            setEnabled(true);
            return;
        }
    }
    // Saving must happen while unlocked: release the lock temporarily, then re-acquire.
    releaseLock();
    if (m_config->save()) {
        updateRestartPrompt();
        emit accepted();
    } else {
        QStringList errors = m_config->lastSaveErrors();
        if (!errors.isEmpty()) {
            QString msg = tr("Save failed due to locked items with changes:\n\n") +
                          errors.join("\n");
            QMessageBox::warning(this, tr("Save Failed"), msg);
        } else {
            QMessageBox::warning(this, tr("Save Failed"), tr("Unknown save error."));
        }
    }
    acquireLock();
    setEnabled(true);
}

void WConfigWidget::onCancelClicked() {
    forEachConfigData([](WConfigDataBase *data) { data->revertToPersistent(); });
    refreshCurrentPage();
    updateRestartPrompt();
    emit rejected();
    close();
}

void WConfigWidget::onAddItemClicked() {
    if (!m_currentViewer ||
        m_currentViewer->effectiveAcceptPolicy() != AcceptPolicy::ACCEPT)
        return;

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Add Configuration Item"));
    QFormLayout *form = new QFormLayout(&dialog);

    QLineEdit *keyEdit = new QLineEdit(&dialog);
    form->addRow(tr("Key:"), keyEdit);

    QComboBox *typeCombo = new QComboBox(&dialog);
    typeCombo->addItems({"String", "Int", "Double", "Bool"});
    form->addRow(tr("Type:"), typeCombo);

    QLineEdit *valueEdit = new QLineEdit(&dialog);
    form->addRow(tr("Value:"), valueEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QString key = keyEdit->text().trimmed();
    if (key.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Key cannot be empty."));
        return;
    }
    if (m_currentViewer->getConfigData(key)) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Item with key '%1' already exists.").arg(key));
        return;
    }

    DataType type;
    QVariant value;
    QString typeStr = typeCombo->currentText();
    bool ok = true;
    if (typeStr == "String") {
        type = DataType::String;
        value = valueEdit->text();
    } else if (typeStr == "Int") {
        type = DataType::Int;
        int intVal = valueEdit->text().toInt(&ok);
        if (!ok) {
            QMessageBox::warning(this, tr("Error"), tr("Invalid integer."));
            return;
        }
        value = intVal;
    } else if (typeStr == "Double") {
        type = DataType::Double;
        double doubleVal = valueEdit->text().toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(this, tr("Error"), tr("Invalid double."));
            return;
        }
        value = doubleVal;
    } else if (typeStr == "Bool") {
        type = DataType::Bool;
        QString text = valueEdit->text().trimmed().toLower();
        if (text == "true" || text == "1" || text == "yes")
            value = true;
        else if (text == "false" || text == "0" || text == "no")
            value = false;
        else {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Invalid boolean (true/false)."));
            return;
        }
    } else
        return;

    WConfigItemInfo info;
    info.displayName(key).defaultValue(value);
    WConfigDataBase *newData = createDataByType(type, key, value, info);
    if (!newData) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to create item."));
        return;
    }

    if (!m_currentViewer->addConfigData(newData)) {
        // Release on failure (e.g. a name clash with a sibling directory) to avoid a leak.
        delete newData;
        QMessageBox::warning(this, tr("Error"),
                             tr("Failed to add item '%1'.").arg(key));
        return;
    }
    refreshCurrentPage();
    updateRestartPrompt();
    m_selectedItemWidget = nullptr;
}

void WConfigWidget::onRemoveItemClicked() {
    if (!m_currentViewer)
        return;
    if (!m_selectedItemWidget) {
        QMessageBox::information(this, tr("Info"),
                                 tr("Please select an item first."));
        return;
    }

    WConfigDataBase *data = m_selectedItemWidget->configData();
    if (!data)
        return;

    DeletionPolicy policy = m_currentViewer->deletionPolicy();
    if (policy == DeletionPolicy::DisallowAll) {
        QMessageBox::warning(this, tr("Deletion Forbidden"),
                             tr("Deletion is not allowed in this directory."));
        return;
    }
    if (policy == DeletionPolicy::AllowNonTemplateOnly &&
        data->isFromTemplate()) {
        QMessageBox::warning(
            this, tr("Cannot Delete Template Item"),
            tr("This item is defined by the template and cannot be deleted."));
        return;
    }

    QString key = data->key();
    QString typeStr;
    switch (data->type()) {
    case DataType::Int:
        typeStr = "int";
        break;
    case DataType::Double:
        typeStr = "Double";
        break;
    case DataType::String:
        typeStr = "String";
        break;
    case DataType::Bool:
        typeStr = "Bool";
        break;
    default:
        typeStr = "Other";
        break;
    }
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Confirm Delete"),
        tr("Are you sure you want to delete the item:\n"
           "Key: %1\nType: %2\nValue: %3")
            .arg(key, typeStr, data->getTemporary().toString()),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes)
        return;

    WConfigViewer *parentViewer = data->parent();
    if (!parentViewer)
        return;
    // Delete only after the item is truly removed from the view, or a dangling data item remains.
    if (!parentViewer->removeConfigData(data, true))
        return;
    delete data;

    refreshCurrentPage();
    updateRestartPrompt();
    m_selectedItemWidget = nullptr;
}

void WConfigWidget::syncCurrentPage() {
    // currentValue() drives each editor's getData(), writing UI values into the data
    // items (group members included).
    for (WConfigItemWidget *widget : std::as_const(m_itemWidgets))
        widget->currentValue();
    for (WConfigGroupWidget *group : std::as_const(m_groupWidgets))
        group->currentValue();
}

void WConfigWidget::refreshDisplay() {
    // Caller-driven refresh: re-read data and update all visible editors.
    for (WConfigItemWidget *w : std::as_const(m_itemWidgets))
        w->refresh();
    for (WConfigGroupWidget *group : std::as_const(m_groupWidgets))
        group->refresh();
    updateRestartPrompt();
}

void WConfigWidget::onConfigChanged(we::config::WConfigDataBase *data) {
    if (!data)
        return;
    if (WConfigItemWidget *w = findItemWidget(data))
        w->refresh();
    updateRestartPrompt();
}

void WConfigWidget::addConfigGroup(WConfigGroupSpec spec) {
    if (!m_currentViewer)
        return;
    if (m_currentViewer->addGroup(spec))
        refreshCurrentPage();
}

} // namespace we::config