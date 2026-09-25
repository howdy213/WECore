/**
 * @file wqssstyle.h
 * @brief Style sheet based style with built-in light and dark themes.
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
#ifndef WQSSSTYLE_H
#define WQSSSTYLE_H

#include "WECore/style/wstyle.h"

namespace we::style {

/**
 * @brief Paints the application with a Qt style sheet.
 *
 * The sheet is generated for the selected theme (light/dark) and applied to the
 * whole application. A style file replaces it completely, while a theme file is
 * appended to it in order to override single rules.
 */
class WE_EXPORT WQssStyle : public WStyle {
    Q_OBJECT
public:
    explicit WQssStyle(QObject *parent = nullptr);

    QString styleName() const override;
    QStringList themes() const override;
    QString theme() const override;
    bool apply() override;
    bool setTheme(const QString &theme) override;

private:
    /// Style sheet to install: selected files if any, otherwise the built-in
    /// sheet for the current theme.
    QString loadStyleSheet() const;
};

} // namespace we::style

#endif // WQSSSTYLE_H