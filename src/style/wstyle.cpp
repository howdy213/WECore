/**
 * @file wstyle.cpp
 * @brief Implementation of the style abstraction and the Style section template.
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
#include "WECore/style/wstyle.h"

#include "WECore/config/WConfig.h"
#include "WECore/config/WConfigItemInfo.h"
#include "WECore/config/WConfigTemplate.h"
#include "WECore/style/wdefaultstyle.h"
#include "WECore/style/wqssstyle.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QStyle>
#include <QStyleFactory>
#include <QWidget>

namespace we::style {

namespace {

/// Installed style; owned by create() and released once the event loop runs.
WStyle *s_active = nullptr;
/// Style the application started with, captured before WStyle changes anything.
QString s_originalStyleKey;

QString originalStyleKey() {
    if (s_originalStyleKey.isEmpty() && QApplication::style())
        s_originalStyleKey = QApplication::style()->objectName();
    return s_originalStyleKey;
}

} // namespace

WStyle::WStyle(QObject *parent) : QObject(parent) { originalStyleKey(); }

WStyle::~WStyle() {
    if (s_active == this)
        s_active = nullptr;
}

QStringList WStyle::availableStyles() {
    QStringList names;
    // "Default" keeps whatever the platform installed, so it always comes first.
    names << QString::fromLatin1(StyleDefault);
    // Everything else Qt can paint with is applied through QStyleFactory, no
    // matter where it comes from: Qt's own styles and the style plugins installed
    // for Qt, including the ones put in place by hand. Listing them straight from
    // QStyleFactory also keeps out styles whose plugin is missing.
    const QStringList keys = QStyleFactory::keys();
    for (const QString &key : keys) {
        if (key.compare(QString::fromLatin1(StyleDefault), Qt::CaseInsensitive) != 0 &&
            key.compare(QString::fromLatin1(StyleQss), Qt::CaseInsensitive) != 0)
            names << key;
    }
    names << QString::fromLatin1(StyleQss);
    return names;
}

WStyle *WStyle::create(const QString &styleName) {
    WStyle *style = nullptr;
    if (styleName.compare(QString::fromLatin1(StyleQss), Qt::CaseInsensitive) == 0) {
        style = new WQssStyle;
    } else if (styleName.compare(QString::fromLatin1(StyleDefault),
                                 Qt::CaseInsensitive) == 0) {
        style = new WDefaultStyle;
    } else {
        // Anything else is expected to be a Qt style: a built-in one or one
        // provided by an installed style plugin. WDefaultStyle applies it, so no
        // plugin needs a wrapper of its own.
        const QString key = pluginStyleKey(styleName);
        if (!key.isEmpty())
            style = new WDefaultStyle(key);
    }
    if (!style) {
        qWarning() << "WStyle::create: unknown style" << styleName;
        return nullptr;
    }
    if (s_active && s_active != style)
        s_active->deleteLater();
    s_active = style;
    return style;
}

WStyle *WStyle::active() { return s_active; }

void WStyle::buildTemplate(we::config::WConfigTemplate &tmpl,
                           const QString &path) {
    tmpl.addSelect(path, QString::fromLatin1(KeyStyleName),
                   we::config::WConfigItemInfo()
                       .defaultValue(QString::fromLatin1(StyleDefault))
                       .displayName(tr("Style"))
                       .description(tr("Selects how the application is painted"))
                       .options(availableStyles()));
    tmpl.addSelect(path, QString::fromLatin1(KeyTheme),
                   we::config::WConfigItemInfo()
                       .defaultValue(QString::fromLatin1(ThemeLight))
                       .displayName(tr("Theme"))
                       .description(tr("Light or dark appearance; styles without "
                                       "themes ignore this"))
                       .options(QStringList()
                                << QString::fromLatin1(ThemeLight)
                                << QString::fromLatin1(ThemeDark)));
    tmpl.addCustom(path, QString::fromLatin1(KeyStyleFile),
                   we::config::WConfigItemInfo()
                       .defaultValue(QString())
                       .displayName(tr("Style file"))
                       .description(tr("Style sheet replacing the built-in theme"))
                       .defaultItem(QStringLiteral("path")));
    tmpl.addCustom(path, QString::fromLatin1(KeyThemeFile),
                   we::config::WConfigItemInfo()
                       .defaultValue(QString())
                       .displayName(tr("Theme file"))
                       .description(tr("Style sheet loaded after the built-in theme"))
                       .defaultItem(QStringLiteral("path")));
    tmpl.setViewerMeta(path, tr("Style"), tr("Application appearance"));
    if (path.isEmpty())
        return;
    // The section owns its directory: refuse items that are not part of the
    // template, so a hand-edited file cannot add entries here.
    tmpl.setDirectoryPolicy(path, we::config::AcceptPolicy::REFUSE,
                            we::config::PolicyPropagation::ApplyToSelfOnly,
                            we::config::OverwriteMode::KeepNonDefault, true);
}

bool WStyle::applyFromValues(const QVariantMap &styleValues) {
    const QString name =
        styleValues.value(QString::fromLatin1(KeyStyleName)).toString();
    // Compared case-insensitively: a name that only differs in case would
    // otherwise keep recreating the (identical) implementation, forever.
    if (!name.isEmpty() && name.compare(styleName(), Qt::CaseInsensitive) != 0) {
        // Another implementation was selected: install it and let it apply the
        // same values. create() only schedules this instance for deletion, so it
        // stays valid (and is no longer touched) after the call.
        WStyle *replacement = create(name);
        if (replacement && replacement != this)
            return replacement->applyFromValues(styleValues);
        qWarning() << "WStyle::applyFromValues: keeping" << styleName()
                   << "because" << name << "is not an available style";
    }
    if (styleValues.contains(QString::fromLatin1(KeyTheme)))
        setTheme(styleValues.value(QString::fromLatin1(KeyTheme)).toString());
    m_styleFile = styleValues.value(QString::fromLatin1(KeyStyleFile)).toString();
    m_themeFile = styleValues.value(QString::fromLatin1(KeyThemeFile)).toString();
    return apply();
}

bool WStyle::selectStyleFile(QWidget *parent, we::config::WConfig *section) {
    const QString file = QFileDialog::getOpenFileName(
        parent, tr("Select Style File"), styleFile(),
        tr("Qt Style Sheets (*.qss);;All Files (*)"));
    if (file.isEmpty())
        return false;
    m_styleFile = file;
    if (section)
        section->setValue(QString::fromLatin1(KeyStyleFile), file);
    return apply();
}

bool WStyle::selectThemeFile(QWidget *parent, we::config::WConfig *section) {
    const QString file = QFileDialog::getOpenFileName(
        parent, tr("Select Theme File"), themeFile(),
        tr("Qt Style Sheets (*.qss);;All Files (*)"));
    if (file.isEmpty())
        return false;
    m_themeFile = file;
    if (section)
        section->setValue(QString::fromLatin1(KeyThemeFile), file);
    return apply();
}

void WStyle::refreshCustomStyleProperty(const QString &theme) {
    const QWidgetList widgets = QApplication::allWidgets();
    for (QWidget *widget : widgets) {
        if (!widget)
            continue;
        QStyle *style = widget->style();
        if (style)
            style->unpolish(widget);
        widget->setProperty("customStyle", theme);
        if (style)
            style->polish(widget);
        widget->update();
    }
}

void WStyle::restoreBaseStyle() {
    const QString key = originalStyleKey();
    if (key.isEmpty())
        return;
    if (QApplication::style() && QApplication::style()->objectName() == key)
        return;
    if (QStyle *style = QStyleFactory::create(key))
        QApplication::setStyle(style);
    else
        qWarning() << "WStyle::restoreBaseStyle: cannot recreate style" << key;
}

QString WStyle::pluginStyleKey(const QString &styleName) {
    const QStringList keys = QStyleFactory::keys();
    for (const QString &key : keys) {
        if (key.compare(styleName, Qt::CaseInsensitive) == 0)
            return key;
    }
    return QString();
}

QString WStyle::readStyleFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "WStyle::readStyleFile: cannot open" << path;
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

} // namespace we::style