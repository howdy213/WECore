/**
 * @file wstyle.h
 * @brief Application appearance abstraction and style switching.
 *
 * A WStyle owns how the application is painted. The default implementation
 * either keeps the style the platform installed or switches to any style
 * QStyleFactory offers, that is Qt's own styles as well as every style plugin
 * installed for Qt, whichever plugin it happens to be. The QSS implementation
 * applies a built-in light/dark style sheet instead.
 *
 * The selection is stored in the "Style" section of the shared configuration
 * (StyleName / Theme / StyleFile / ThemeFile). A style holds no config of its
 * own: the caller builds that section from buildTemplate() into a sub-config of
 * its own and mounts it into the config that owns config.json, so a settings
 * dialog edits and persists the appearance together with the rest of the file.
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
#ifndef WSTYLE_H
#define WSTYLE_H

#include "WECore/def/wedef.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

class QWidget;

namespace we::config {
class WConfig;
class WConfigTemplate;
} // namespace we::config

namespace we::style {

/// Identifiers of the built-in styles, as stored in "Style/StyleName".
/// Any other value stored there is the key of a style QStyleFactory provides.
inline constexpr auto StyleDefault = "Default";
inline constexpr auto StyleQss = "QSS";

/// Values of "Style/Theme"; styles that do not support themes ignore them.
inline constexpr auto ThemeLight = "light";
inline constexpr auto ThemeDark = "dark";

/// Keys of the "Style" section inside config/config.json.
inline constexpr auto KeyStyleName = "StyleName";
inline constexpr auto KeyTheme = "Theme";
inline constexpr auto KeyStyleFile = "StyleFile";
inline constexpr auto KeyThemeFile = "ThemeFile";

/**
 * @brief Base class of every style implementation.
 *
 * Concrete styles are created through create(), which also installs the new
 * instance as the active one; active() returns it. A style is stateless with
 * respect to the configuration tree: switching the implementation changes how
 * the application is painted, never the "Style" section itself.
 */
class WE_EXPORT WStyle : public QObject {
    Q_OBJECT
public:
    /// Style names that can be selected: the built-in ones plus every key
    /// QStyleFactory knows, that is Qt's own styles and the style plugins
    /// installed for Qt (whether they ship with the application or were placed
    /// by hand).
    static QStringList availableStyles();
    /// Creates and installs the style named @p styleName, deleting the previously
    /// installed instance. Besides the built-in names this accepts any key
    /// QStyleFactory knows. Returns nullptr for an unknown name.
    static WStyle *create(const QString &styleName);
    /// The installed style, or nullptr before the first successful create().
    static WStyle *active();

    explicit WStyle(QObject *parent = nullptr);
    ~WStyle() override;

    /// Identifier of this implementation (one of the Style* constants).
    virtual QString styleName() const = 0;
    /// Themes this implementation can switch between; empty if it has none.
    virtual QStringList themes() const = 0;
    /// Currently selected theme.
    virtual QString theme() const = 0;
    /// Applies the style to the whole application.
    virtual bool apply() = 0;
    /// Selects @p theme and re-applies it; returns false for an unknown theme.
    virtual bool setTheme(const QString &theme) = 0;

    /// Lets the user pick a style sheet file; the selection is applied
    /// immediately and written into the "StyleFile" item of @p section, so it is
    /// persisted when the config carrying the Style section is saved. Pass a null
    /// @p section to apply the selection without storing it.
    bool selectStyleFile(QWidget *parent, we::config::WConfig *section);
    /// Lets the user pick a style sheet fragment that is loaded after the
    /// built-in sheet, so it can override individual rules of the current theme.
    /// Stored as "ThemeFile" in @p section, same as selectStyleFile().
    bool selectThemeFile(QWidget *parent, we::config::WConfig *section);

    /**
     * @brief Fills @p tmpl with the items of the Style section.
     *
     * A style never holds a config itself: the caller creates the sub-config that
     * carries the section from this template and mounts it into the config owning
     * config.json. The template has to stay alive as long as that config is used,
     * because the document only keeps a pointer to it.
     *
     * @p path is the directory the section lives in: pass e.g. "Style" to declare
     * it as its own directory including its display name and directory policy, or
     * leave it empty to put the items in the root of @p tmpl.
     */
    static void buildTemplate(we::config::WConfigTemplate &tmpl,
                              const QString &path = QString());
    /// Applies the style described by @p styleValues (the flat contents of the
    /// Style section) and switches the implementation if StyleName asks for it.
    bool applyFromValues(const QVariantMap &styleValues);

protected:
    /// File selected as the full style sheet ("" = use the built-in one).
    QString styleFile() const { return m_styleFile; }
    /// File selected as an override sheet loaded after the built-in one.
    QString themeFile() const { return m_themeFile; }
    /// Marks every existing widget with the current theme and repolishes it, so
    /// rules keyed on the "customStyle" property react immediately.
    static void refreshCustomStyleProperty(const QString &theme);
    /// Switches back to the style the application started with.
    static void restoreBaseStyle();
    /// Matching key inside QStyleFactory::keys() for @p styleName, or an empty
    /// string when no installed Qt style plugin provides it.
    static QString pluginStyleKey(const QString &styleName);
    /// Reads @p path as UTF-8 text; returns an empty string when unreadable.
    static QString readStyleFile(const QString &path);

    /// Theme of this instance, kept by the subclasses that support themes.
    QString m_theme = QString::fromLatin1(ThemeLight);

private:
    QString m_styleFile;
    QString m_themeFile;
};

} // namespace we::style

#endif // WSTYLE_H