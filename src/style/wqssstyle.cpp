/**
 * @file wqssstyle.cpp
 * @brief Implementation of the style sheet based style.
 *
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
#include "WECore/style/wqssstyle.h"

#include <QApplication>
#include <QDebug>

namespace we::style {

namespace {

/// Colors of one theme, used to fill the placeholders of the sheet below.
struct ThemeColors {
    QString window;
    QString base;
    QString text;
    QString textDisabled;
    QString border;
    QString hover;
    QString pressed;
    QString accent;
    QString altBase;
};

ThemeColors themeColors(bool dark) {
    ThemeColors colors;
    if (dark) {
        colors.window = QStringLiteral("rgba(30, 34, 44, 255)");
        colors.base = QStringLiteral("rgba(24, 28, 36, 255)");
        colors.text = QStringLiteral("rgba(226, 230, 238, 255)");
        colors.textDisabled = QStringLiteral("rgba(122, 128, 142, 255)");
        colors.border = QStringLiteral("rgba(58, 64, 80, 255)");
        colors.hover = QStringLiteral("rgba(46, 52, 66, 255)");
        colors.pressed = QStringLiteral("rgba(58, 66, 84, 255)");
        colors.accent = QStringLiteral("rgba(96, 130, 255, 255)");
        colors.altBase = QStringLiteral("rgba(38, 43, 55, 255)");
    } else {
        colors.window = QStringLiteral("rgba(245, 246, 249, 255)");
        colors.base = QStringLiteral("rgba(255, 255, 255, 255)");
        colors.text = QStringLiteral("rgba(35, 39, 47, 255)");
        colors.textDisabled = QStringLiteral("rgba(150, 156, 168, 255)");
        colors.border = QStringLiteral("rgba(206, 211, 222, 255)");
        colors.hover = QStringLiteral("rgba(232, 236, 245, 255)");
        colors.pressed = QStringLiteral("rgba(218, 224, 238, 255)");
        colors.accent = QStringLiteral("rgba(76, 110, 245, 255)");
        colors.altBase = QStringLiteral("rgba(238, 241, 246, 255)");
    }
    return colors;
}

/// Placeholders: %1 window, %2 base, %3 text, %4 disabled text, %5 border,
/// %6 hover, %7 pressed, %8 accent, %9 alternate base.
QString builtinSheet(bool dark) {
    const ThemeColors c = themeColors(dark);
    return QString::fromLatin1(R"(
QWidget {
    background-color: %1;
    color: %3;
}
QWidget:disabled {
    color: %4;
}
QMainWindow, QDialog, QMenuBar, QMenu, QToolBar, QStatusBar, QDockWidget {
    background-color: %1;
    color: %3;
}
QLabel, QCheckBox, QRadioButton {
    background-color: transparent;
}
QMenuBar::item {
    background: transparent;
    padding: 4px 8px;
}
QMenuBar::item:selected, QMenu::item:selected {
    background-color: %6;
}
QMenu::separator {
    height: 1px;
    background-color: %5;
    margin: 4px 6px;
}
QPushButton, QToolButton {
    background-color: %2;
    color: %3;
    border: 1px solid %5;
    border-radius: 4px;
    padding: 5px 12px;
}
QPushButton:hover, QToolButton:hover {
    background-color: %6;
}
QPushButton:pressed, QToolButton:pressed {
    background-color: %7;
}
QPushButton:disabled, QToolButton:disabled {
    background-color: %1;
    color: %4;
}
QPushButton:default {
    border-color: %8;
}
QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QDoubleSpinBox, QComboBox {
    background-color: %2;
    color: %3;
    border: 1px solid %5;
    border-radius: 4px;
    padding: 4px 6px;
    selection-background-color: %8;
    selection-color: #ffffff;
}
QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus, QSpinBox:focus,
QDoubleSpinBox:focus, QComboBox:focus {
    border-color: %8;
}
QLineEdit:disabled, QTextEdit:disabled, QPlainTextEdit:disabled,
QSpinBox:disabled, QDoubleSpinBox:disabled, QComboBox:disabled {
    background-color: %1;
    color: %4;
}
QComboBox::drop-down {
    border: none;
    width: 18px;
}
QComboBox QAbstractItemView {
    background-color: %2;
    color: %3;
    border: 1px solid %5;
    selection-background-color: %8;
    selection-color: #ffffff;
}
QAbstractItemView {
    background-color: %2;
    color: %3;
    border: 1px solid %5;
    border-radius: 4px;
    alternate-background-color: %9;
    selection-background-color: %8;
    selection-color: #ffffff;
}
QTreeView::item, QListView::item {
    padding: 3px 4px;
}
QTreeView::item:hover, QListView::item:hover {
    background-color: %6;
}
QHeaderView::section {
    background-color: %9;
    color: %3;
    border: none;
    border-right: 1px solid %5;
    border-bottom: 1px solid %5;
    padding: 4px 6px;
}
QTabWidget::pane {
    background-color: %1;
    border: 1px solid %5;
    border-radius: 4px;
}
QTabBar::tab {
    background-color: %9;
    color: %3;
    border: 1px solid %5;
    border-bottom: none;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
    padding: 5px 12px;
    margin-right: 2px;
}
QTabBar::tab:hover {
    background-color: %6;
}
QTabBar::tab:selected {
    background-color: %2;
}
QGroupBox {
    border: 1px solid %5;
    border-radius: 4px;
    margin-top: 10px;
    padding: 8px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 8px;
    padding: 0 4px;
    color: %3;
}
QCheckBox, QRadioButton {
    spacing: 6px;
}
QSlider::groove:horizontal {
    background-color: %5;
    height: 4px;
    border-radius: 2px;
}
QSlider::sub-page:horizontal {
    background-color: %8;
    border-radius: 2px;
}
QSlider::handle:horizontal {
    background-color: %2;
    border: 1px solid %5;
    width: 12px;
    margin: -5px 0;
    border-radius: 6px;
}
QScrollBar:vertical {
    background-color: transparent;
    width: 12px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background-color: %5;
    min-height: 24px;
    border-radius: 5px;
}
QScrollBar::handle:vertical:hover {
    background-color: %4;
}
QScrollBar:horizontal {
    background-color: transparent;
    height: 12px;
    margin: 0;
}
QScrollBar::handle:horizontal {
    background-color: %5;
    min-width: 24px;
    border-radius: 5px;
}
QScrollBar::handle:horizontal:hover {
    background-color: %4;
}
QScrollBar::add-line, QScrollBar::sub-line {
    width: 0;
    height: 0;
}
QScrollBar::add-page, QScrollBar::sub-page {
    background-color: transparent;
}
QProgressBar {
    background-color: %2;
    color: %3;
    border: 1px solid %5;
    border-radius: 4px;
    text-align: center;
}
QProgressBar::chunk {
    background-color: %8;
    border-radius: 3px;
}
QToolTip {
    background-color: %2;
    color: %3;
    border: 1px solid %5;
    padding: 2px 4px;
}
QSplitter::handle {
    background-color: %5;
}
QStatusBar::item {
    border: none;
}
)")
        .arg(c.window, c.base, c.text, c.textDisabled, c.border, c.hover,
             c.pressed, c.accent, c.altBase);
}

} // namespace

WQssStyle::WQssStyle(QObject *parent) : WStyle(parent) {}

QString WQssStyle::styleName() const { return QString::fromLatin1(StyleQss); }

QStringList WQssStyle::themes() const {
    return QStringList() << QString::fromLatin1(ThemeLight)
                         << QString::fromLatin1(ThemeDark);
}

QString WQssStyle::theme() const { return m_theme; }

bool WQssStyle::setTheme(const QString &theme) {
    if (!themes().contains(theme, Qt::CaseInsensitive)) {
        qWarning() << "WQssStyle::setTheme: unknown theme" << theme;
        return false;
    }
    if (m_theme.compare(theme, Qt::CaseInsensitive) == 0)
        return true;
    m_theme = theme.toLower();
    return apply();
}

bool WQssStyle::apply() {
    // A style sheet is meant to replace the platform look, so first drop any
    // style another implementation installed. setStyleSheet() is a member of
    // QApplication, so it has to be called on the application instance.
    restoreBaseStyle();
    qApp->setStyleSheet(loadStyleSheet());
    refreshCustomStyleProperty(theme());
    return true;
}

QString WQssStyle::loadStyleSheet() const {
    if (!styleFile().isEmpty()) {
        const QString custom = readStyleFile(styleFile());
        if (!custom.isEmpty())
            return custom;
        qWarning() << "WQssStyle: cannot read style file" << styleFile()
                   << "- falling back to the built-in sheet";
    }
    const bool dark = (theme().compare(QString::fromLatin1(ThemeDark),
                                       Qt::CaseInsensitive) == 0);
    QString sheet = builtinSheet(dark);
    if (!themeFile().isEmpty()) {
        const QString extra = readStyleFile(themeFile());
        if (!extra.isEmpty()) {
            sheet += QLatin1Char('\n');
            sheet += extra;
        }
    }
    return sheet;
}

} // namespace we::style