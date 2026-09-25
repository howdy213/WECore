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
#include "WECore/config/WConfigCustomBuiltins.h"
#include "WECore/config/WConfigCustomType.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>

namespace we::config {

WConfigPathEditor::WConfigPathEditor(QWidget *parent) : QWidget(parent) {
    m_edit = new QLineEdit(this);
    m_edit->setPlaceholderText(tr("Browse..."));
    m_edit->setClearButtonEnabled(true);
    QPushButton *browseBtn = new QPushButton(tr("..."), this);
    browseBtn->setFixedWidth(40);
    connect(browseBtn, &QPushButton::clicked, this, &WConfigPathEditor::onBrowse);
    // Manual editing should also be treated as a value change
    connect(m_edit, &QLineEdit::textChanged, this, [this](const QString &) {
        emit valueEdited();
    });

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_edit, 1);
    layout->addWidget(browseBtn);
    setLayout(layout);
}

QVariant WConfigPathEditor::editValue() const { return m_edit->text(); }

void WConfigPathEditor::setEditValue(const QVariant &v) {
    QString text = v.toString();
    if (m_edit->text() == text)
        return;
    m_edit->blockSignals(true);
    m_edit->setText(text);
    m_edit->blockSignals(false);
}

void WConfigPathEditor::setDirectoryMode(bool isDir) { m_directory = isDir; }

void WConfigPathEditor::onBrowse() {
    QString start = m_edit->text();
    QString result = m_directory
        ? QFileDialog::getExistingDirectory(this, tr("Select Directory"), start)
        : QFileDialog::getOpenFileName(this, tr("Select File"), start);
    if (result.isEmpty())
        return;
    setEditValue(result);
    emit valueEdited();
}

void registerBuiltinCustomTypes() {
    WConfigCustomTypeRegistry &reg = WConfigCustomTypeRegistry::instance();

    WConfigCustomType path;
    path.typeName = QStringLiteral("path");
    path.serialize = [](const QVariant &v) { return v.toString(); };
    path.deserialize = [](const QVariant &v) { return v.toString(); };
    path.defaultValue = QString();
    path.displayString = [](const QVariant &v) { return v.toString(); };
    path.editorFactory = [](QWidget *parent, WCustomEditorInterface **out) {
        WConfigPathEditor *editor = new WConfigPathEditor(parent);
        if (out)
            *out = editor;
        return static_cast<QWidget *>(editor);
    };
    reg.registerType(path);
}

} // namespace we::config