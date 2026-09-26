/**
 * @file wfluentstyle.cpp
 * @brief Implementation of the style driving the FluentUI3 style plugin.
 *
 * @author howdy213
 * @date 2026-09-26
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
#include "WECore/style/wfluentstyle.h"

#include "WECore/config/WConfigItemInfo.h"
#include "WECore/config/WConfigTemplate.h"

#include <QApplication>
#include <QDebug>
#include <QStyle>
#include <QStyleFactory>

namespace we::style {

namespace {

/// Property the FluentUI3 style reads to choose its light/dark appearance: 0 is
/// light, 1 is dark.
constexpr auto ColorSchemeProperty = "_q_colorscheme";
/// Property it reads to choose its color theme, as an index into
/// colorThemeOptions().
constexpr auto ThemeStyleProperty = "_q_themestyle";
/// Property it reads to override the accent color of its color theme.
constexpr auto AccentColorProperty = "_q_accent_color";

/// Value of the color scheme property that makes FluentUI3 paint its light
/// appearance.
constexpr int ColorSchemeLight = 0;
/// Value of the color scheme property that makes FluentUI3 paint its dark
/// appearance.
constexpr int ColorSchemeDark = 1;

} // namespace

WFluentUIStyle::WFluentUIStyle(QObject *parent) : WStyle(parent) {}

QString WFluentUIStyle::styleName() const {
    return QString::fromLatin1(StyleFluentUI3);
}

QStringList WFluentUIStyle::themeOptions() {
    return QStringList() << QString::fromLatin1(ThemeLight)
                         << QString::fromLatin1(ThemeDark);
}

QStringList WFluentUIStyle::colorThemeOptions() {
    return QStringList() << QString::fromLatin1(ColorThemeFluent)
                         << QString::fromLatin1(ColorThemeTeams);
}

QStringList WFluentUIStyle::themes() const { return themeOptions(); }

QStringList WFluentUIStyle::colorThemes() const { return colorThemeOptions(); }

QString WFluentUIStyle::theme() const { return m_theme; }

void WFluentUIStyle::buildTemplate(we::config::WConfigTemplate &tmpl,
                                   const QString &path) {
    tmpl.addCustom(path, QString::fromLatin1(KeyAccentColor),
                   we::config::WConfigItemInfo()
                       .defaultValue(QString())
                       .displayName(tr("Accent color"))
                       .description(tr("Overrides the accent color of the color "
                                       "theme; empty keeps it"))
                       .defaultItem(QStringLiteral("color")));
}

void WFluentUIStyle::readValues(const QVariantMap &styleValues) {
    const QString colorTheme =
        styleValues.value(QString::fromLatin1(KeyColorTheme)).toString();
    if (!colorTheme.isEmpty()) {
        const int index = colorThemes().indexOf(colorTheme, Qt::CaseInsensitive);
        if (index < 0) {
            qWarning() << "WFluentUIStyle::readValues: unknown color theme"
                       << colorTheme;
        } else {
            m_colorThemeIndex = index;
        }
    }
    // An empty or malformed value means "no override", which is not reported: the
    // accent color of the color theme is used then.
    m_accentColor = QColor(
        styleValues.value(QString::fromLatin1(KeyAccentColor)).toString());
}

bool WFluentUIStyle::setTheme(const QString &theme) {
    if (!themes().contains(theme, Qt::CaseInsensitive)) {
        qWarning() << "WFluentUIStyle::setTheme: unknown theme" << theme;
        return false;
    }
    if (m_theme.compare(theme, Qt::CaseInsensitive) == 0)
        return true;
    m_theme = theme.toLower();
    return apply();
}

bool WFluentUIStyle::apply() {
    // FluentUI3 paints every control itself, so a style sheet left behind by the
    // QSS implementation would only fight it. setStyleSheet() is a member of
    // QApplication, so it has to be called on the application instance.
    qApp->setStyleSheet(QString());

    // FluentUI3 reads all three while it is being constructed, so they are set
    // before the style is created; creating the style anew is also how the style
    // itself expects them to change.
    const bool dark = (m_theme.compare(QString::fromLatin1(ThemeDark),
                                       Qt::CaseInsensitive) == 0);
    qApp->setProperty(ColorSchemeProperty,
                      dark ? ColorSchemeDark : ColorSchemeLight);
    qApp->setProperty(ThemeStyleProperty, m_colorThemeIndex);
    qApp->setProperty(AccentColorProperty,
                      m_accentColor.isValid() ? QVariant(m_accentColor)
                                              : QVariant());

    bool applied = true;
    // The key the plugin registered, as QStyleFactory reports it, rather than the
    // name this class uses for itself.
    if (QStyle *style = QStyleFactory::create(pluginStyleKey(styleName()))) {
        QApplication::setStyle(style);
    } else {
        // The dll is gone or cannot be loaded. Restoring the platform style keeps
        // the application usable, and the failure is still reported because the
        // selection could not be honoured. The properties are dropped with it so
        // that no stray values are left behind for the style taking over.
        qApp->setProperty(ColorSchemeProperty, QVariant());
        qApp->setProperty(ThemeStyleProperty, QVariant());
        qApp->setProperty(AccentColorProperty, QVariant());
        qWarning() << "WFluentUIStyle: the FluentUI3 style plugin is not "
                      "available; restoring the platform style";
        restoreBaseStyle();
        applied = false;
    }
    refreshCustomStyleProperty(m_theme);
    return applied;
}

} // namespace we::style