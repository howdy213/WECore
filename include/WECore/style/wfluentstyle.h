/**
 * @file wfluentstyle.h
 * @brief Style implementation driving the FluentUI3 style plugin.
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
#ifndef WFLUENTSTYLE_H
#define WFLUENTSTYLE_H

#include "WECore/style/wstyle.h"

#include <QColor>

namespace we::config {
class WConfigTemplate;
} // namespace we::config

namespace we::style {

/// Entry of the "Style" section this implementation owns. Its other entry, the
/// color theme, belongs to every style (KeyColorTheme) and is only filled from
/// here.
inline constexpr auto KeyAccentColor = "AccentColor";

/// Values of "Style/ColorTheme" this implementation offers: the palette family the
/// FluentUI3 style derives its colors from. "Fluent" is the Windows 11 accent
/// system, "Teams" the Teams one. The list is not closed: a palette family the
/// plugin gains only has to be added to colorThemeOptions(), because the position
/// of a value in that list is the index the plugin is given.
inline constexpr auto ColorThemeFluent = "Fluent";
inline constexpr auto ColorThemeTeams = "Teams";

/**
 * @brief Applies the FluentUI3 style, taken from its plugin and nothing else.
 *
 * The plugin is used through the key it registers with Qt, so nothing here links
 * against FluentUI3Style or includes its headers: deploying the style plugin dll
 * is enough, and when the dll is missing the style is simply not offered, because
 * it does not show up in QStyleFactory::keys() either.
 *
 * QStyleFactory alone cannot drive that style. FluentUI3 takes its light/dark
 * appearance, its color theme and its accent color from properties of the
 * application while it is being constructed, so this implementation sets those
 * properties up and then creates the style, which is also how the style itself
 * expects those three to change. The appearance and the color theme are the
 * entries every style shares (Theme and ColorTheme), offered with the values
 * reported below; the accent color is an entry of this implementation, see
 * buildTemplate().
 */
class WE_EXPORT WFluentUIStyle : public WStyle {
    Q_OBJECT
public:
    explicit WFluentUIStyle(QObject *parent = nullptr);

    QString styleName() const override;
    QStringList themes() const override;
    QStringList colorThemes() const override;
    QString theme() const override;
    bool apply() override;
    bool setTheme(const QString &theme) override;

    /// Themes this implementation can be switched to, asked for by
    /// WStyle::buildTemplate() before any instance exists; themes() reports the
    /// same values. FluentUI3 takes them from its color scheme property.
    static QStringList themeOptions();
    /// Color themes this implementation offers, asked for by
    /// WStyle::buildTemplate() the same way; colorThemes() reports the same values.
    static QStringList colorThemeOptions();

    /**
     * @brief Adds the entry of the "Style" section this implementation owns: the
     * accent color.
     *
     * Called from WStyle::buildTemplate() only when the plugin is installed for
     * Qt, @p path being the directory the section lives in ("" for the root).
     */
    static void buildTemplate(we::config::WConfigTemplate &tmpl,
                              const QString &path);

protected:
    void readValues(const QVariantMap &styleValues) override;

private:
    /// Color theme selected as "Style/ColorTheme", as the index FluentUI3 expects
    /// for it: the position of the name in colorThemeOptions().
    int m_colorThemeIndex = 0;
    /// Accent color selected as "Style/AccentColor"; invalid when unset, in which
    /// case the color theme keeps its own accent color.
    QColor m_accentColor;
};

} // namespace we::style

#endif // WFLUENTSTYLE_H