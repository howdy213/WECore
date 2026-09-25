/**
 * @file wdefaultstyle.h
 * @brief Style implementation applying a style provided by Qt itself.
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
#ifndef WDEFAULTSTYLE_H
#define WDEFAULTSTYLE_H

#include "WECore/style/wstyle.h"

namespace we::style {

/**
 * @brief Applies a style provided by Qt itself.
 *
 * With an empty key it keeps the style the platform installed and only undoes
 * what other styles changed, which also makes it the fallback whenever a style
 * cannot be applied. With a non-empty key it switches to that key inside
 * QStyleFactory, covering Qt's built-in styles as well as any style plugin
 * installed for Qt (qlementine, for instance, but nothing here is specific to
 * it).
 *
 * It has no light/dark themes of its own.
 */
class WE_EXPORT WDefaultStyle : public WStyle {
    Q_OBJECT
public:
    explicit WDefaultStyle(const QString &styleKey = QString(),
                           QObject *parent = nullptr);

    QString styleName() const override;
    QStringList themes() const override;
    QString theme() const override;
    bool apply() override;
    bool setTheme(const QString &theme) override;

private:
    /// Key inside QStyleFactory; empty means "keep the style the platform
    /// installed".
    QString m_styleKey;
};

} // namespace we::style

#endif // WDEFAULTSTYLE_H