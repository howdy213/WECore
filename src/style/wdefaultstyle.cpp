/**
 * @file wdefaultstyle.cpp
 * @brief Implementation of the style that applies a Qt-provided style.
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
#include "WECore/style/wdefaultstyle.h"

#include <QApplication>
#include <QDebug>
#include <QStyle>
#include <QStyleFactory>

namespace we::style {

WDefaultStyle::WDefaultStyle(const QString &styleKey, QObject *parent)
    : WStyle(parent), m_styleKey(styleKey) {}

QString WDefaultStyle::styleName() const {
    return m_styleKey.isEmpty() ? QString::fromLatin1(StyleDefault) : m_styleKey;
}

QStringList WDefaultStyle::themes() const { return QStringList(); }

QString WDefaultStyle::theme() const { return QString(); }

bool WDefaultStyle::apply() {
    // A style sheet left behind by another implementation would keep overriding
    // the Qt style, so it is always cleared first. setStyleSheet() is a member of
    // QApplication, so it has to be called on the application instance.
    qApp->setStyleSheet(QString());

    bool applied = true;
    if (m_styleKey.isEmpty()) {
        // No explicit selection: keep whatever style the platform installed.
        restoreBaseStyle();
    } else if (QStyle *style = QStyleFactory::create(m_styleKey)) {
        // A key QStyleFactory knows, be it one of Qt's own styles or one provided
        // by an installed style plugin.
        QApplication::setStyle(style);
    } else {
        // The plugin that provided the selected style is gone. Restoring the
        // platform style keeps the application usable, and the failure is still
        // reported because the selection could not be honoured.
        qWarning() << "WDefaultStyle: no Qt style named" << m_styleKey
                   << "; restoring the platform style";
        restoreBaseStyle();
        applied = false;
    }
    refreshCustomStyleProperty(QString());
    return applied;
}

bool WDefaultStyle::setTheme(const QString &theme) {
    // Qt styles have no light/dark theme of their own; the value is still kept in
    // the config so that a theme-aware style selected later can pick it up.
    return theme.isEmpty();
}

} // namespace we::style