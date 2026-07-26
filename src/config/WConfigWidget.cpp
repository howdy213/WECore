#include "WECore/config/WConfigWidget.h"
#include "WECore/config/WConfigDataDef.h"
#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <functional>

namespace we::config {

WConfigWidget::WConfigWidget(WConfig *config, QWidget *parent)
    : QDialog(parent), m_config(config), m_treeWidget(new QTreeWidget),
    m_scrollArea(new QScrollArea), m_contentWidget(new QWidget),
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
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setMinimumWidth(200);
    splitter->addWidget(m_treeWidget);

    // 构建树：先创建根节点
    m_rootItem = new QTreeWidgetItem();
    WConfigViewer *rootViewer = m_config->document()->root();
    QString rootDisplay = rootViewer->displayName();
    if (rootDisplay.isEmpty())
        rootDisplay = tr("Root");
    m_rootItem->setText(0, rootDisplay);
    m_rootItem->setToolTip(0, rootViewer->description());
    m_rootItem->setData(0, Qt::UserRole, "");  // 空路径表示根
    m_treeWidget->addTopLevelItem(m_rootItem);
    m_treeWidget->expandItem(m_rootItem);      // 初始展开

    // 强制根节点永远展开
    connect(m_treeWidget, &QTreeWidget::itemCollapsed, this, [this](QTreeWidgetItem *item) {
        if (item == m_rootItem) {
            m_treeWidget->expandItem(item);   // 立刻重新展开
        }
    });

    // 添加所有子目录
    buildTree(rootViewer, m_rootItem);

    // 默认选中根节点并显示其内容
    m_treeWidget->setCurrentItem(m_rootItem);
    onTreeItemClicked(m_rootItem, 0);

    connect(m_treeWidget, &QTreeWidget::itemClicked, this,
            &WConfigWidget::onTreeItemClicked);

    m_scrollArea->setWidgetResizable(true);
    m_contentLayout->setAlignment(Qt::AlignTop);
    m_scrollArea->setWidget(m_contentWidget);
    splitter->addWidget(m_scrollArea);
    splitter->setSizes({200, 600});
    mainLayout->addWidget(splitter);

    // 底部按钮行
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    m_restartLabel->setStyleSheet("color: #e67e22;");
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

    if (m_config && m_config->document() && m_config->document()->root()) {
        m_config->document()->root()->lock();
    }
    updateRestartPrompt();
}

WConfigWidget::~WConfigWidget() {
    if (m_config->document() && m_config->document()->root()) {
        m_config->document()->root()->unlock();
    }
}

void WConfigWidget::addVerification(std::function<bool(WConfig*)> func)
{
    if (func) {
        m_verifications.append(func);
    }
}

void WConfigWidget::forEachConfigData(
    std::function<void(WConfigDataBase *)> func) {
    std::function<void(WConfigViewer *)> traverse = [&](WConfigViewer *viewer) {
        if (!viewer)
            return;
        for (WConfigDataBase *data : viewer->allConfigData()) {
            func(data);
            // 不建议对Object内部使用，库不保证Object内部设置功能可用性
            data->forEachChild(func); // 递归遍历子项
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

    // 清空布局
    QLayoutItem *child;
    while ((child = m_contentLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QString path = item->data(0, Qt::UserRole).toString();
    WConfigViewer *viewer = nullptr;
    if (path.isEmpty()) {
        viewer = m_config->document()->root();   // 根节点
    } else {
        viewer = m_config->document()->root()->findChildViewer(path);
    }
    if (viewer)
        displayConfig(viewer);
}

void WConfigWidget::displayConfig(WConfigViewer *viewer) {
    m_currentViewer = viewer;
    m_itemWidgets.clear();

    for (WConfigDataBase *data : viewer->allConfigData()) {
        WConfigItemWidget *itemWidget = new WConfigItemWidget(data);
        m_contentLayout->addWidget(itemWidget);
        m_itemWidgets.append(itemWidget);
        connect(itemWidget, &WConfigItemWidget::clicked, this,
                [this](WConfigItemWidget *w) {
                    if (m_selectedItemWidget == w)
                return;
                    for (auto *item : m_itemWidgets) {
                item->setSelected(false);
                    }
                    w->setSelected(true);
                    m_selectedItemWidget = w;
        });
        connect(itemWidget, &WConfigItemWidget::valueChanged, this,
                &WConfigWidget::updateRestartPrompt);
    }
    m_contentLayout->addStretch();

    setupAcceptPolicyButtons(viewer);
    updateRestartPrompt(); // 每次显示刷新重启提示
}

void WConfigWidget::refreshCurrentPage() {
    if (m_currentViewer) {
        // 清空并重新显示
        QLayoutItem *child;
        while ((child = m_contentLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
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
    // 同步当前页编辑器
    setEnabled(false);
    syncCurrentPage();
    for (const auto& verify : m_verifications) {
        if (!verify(m_config)) {
            setEnabled(true);
            return;
        }
    }
    if (m_config->document() && m_config->document()->root()) {
        m_config->document()->root()->unlock();
    }
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
    if (m_config->document() && m_config->document()->root()) {
        m_config->document()->root()->lock();
    }
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

    m_currentViewer->addConfigData(newData);
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

    // 确认对话框
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
    parentViewer->removeConfigData(data);
    delete data;

    refreshCurrentPage();
    updateRestartPrompt();
    m_selectedItemWidget = nullptr;
}

void WConfigWidget::syncCurrentPage() {
    for (WConfigItemWidget *widget : m_itemWidgets) {
        // 调用 currentValue() 会触发编辑器内部的 getData()，从而将界面值写入数据项
        widget->currentValue();
    }
}

} // namespace we::config