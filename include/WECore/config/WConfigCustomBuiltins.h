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
#ifndef WCONFIGCUSTOMBUILTINS_H
#define WCONFIGCUSTOMBUILTINS_H

#include "WECore/def/wedef.h"
#include "WConfigCustomType.h"
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace we::config {

// Path browse editor: QLineEdit + browse button.
class WE_EXPORT WConfigPathEditor : public QWidget, public WCustomEditorInterface {
    Q_OBJECT
public:
    explicit WConfigPathEditor(QWidget *parent = nullptr);

    QVariant editValue() const override;         // Returns the current path string
    void setEditValue(const QVariant &v) override; // Sets the path (does not emit valueEdited)

    // Browse mode: true = directory selection (QFileDialog::getExistingDirectory), false = file selection
    void setDirectoryMode(bool isDir);

signals:
    void valueEdited();

private slots:
    void onBrowse();

private:
    QLineEdit *m_edit = nullptr;
    bool m_directory = true;
};

// Color picker editor: a #RRGGBB value in a line edit, next to a swatch that opens QColorDialog.
class WE_EXPORT WConfigColorEditor : public QWidget, public WCustomEditorInterface {
    Q_OBJECT
public:
    explicit WConfigColorEditor(QWidget *parent = nullptr);

    QVariant editValue() const override;           // Returns the current color as "#rrggbb"
    void setEditValue(const QVariant &v) override; // Sets the color (does not emit valueEdited)

signals:
    void valueEdited();

private slots:
    void onPick();

private:
    void updateSwatch();

    QLineEdit *m_edit = nullptr;
    QPushButton *m_swatch = nullptr;
};

// Registers the built-in custom types (such as "path" and "color"). Called on
// first use of the registry instance().
WE_EXPORT void registerBuiltinCustomTypes();

} // namespace we::config

#endif // WCONFIGCUSTOMBUILTINS_H