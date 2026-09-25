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
#ifndef WCONFIGGROUPWIDGET_H
#define WCONFIGGROUPWIDGET_H

#include "WConfigViewer.h"
#include <QHash>
#include <QLabel>
#include <QLayout>
#include <QFrame>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>

namespace we::config {

class WConfigItemWidget;

// Merged-config rendering widget: several config items share one title, rendered
// per WConfigGroupSpec. Each member uses the framework's own item style (Inline
// keeps its own title), so group titles/editors align naturally; the member area
// has an indent and a vertical guide to convey the group hierarchy. A fully custom
// member layout is supplied at creation via WConfigGroupSpec::layoutBuilder.
class WE_EXPORT WConfigGroupWidget : public QWidget {
    Q_OBJECT
public:
    WConfigGroupWidget(WConfigViewer *viewer, WConfigGroupSpec *spec,
                       QWidget *parent = nullptr);

    WConfigGroupSpec *groupSpec() const { return m_spec; }
    // Caller-driven refresh: re-read data into all member editors.
    void refresh();
    // Rebuild all member rows from m_spec->members and m_visible.
    void applySpec();
    // Change one member's render mode (updates the spec, rebuilds immediately).
    void setItemMode(const QString &key, WConfigItemMode mode);
    // Show/hide one member (rebuilds immediately).
    void setItemVisible(const QString &key, bool on);
    QList<WConfigItemWidget *> memberWidgets() const;
    // Flush all member editors into the data.
    void currentValue() const;
    // Set the group description (shown as the title tooltip).
    void setGroupDescription(const QString &desc);

    // ---- For layout builders / callers to access at creation time ----
    QLabel *titleWidget() const { return m_titleLabel; }
    void setTitleVisible(bool on);
    // Current member-area layout (from layoutBuilder or the default vertical layout).
    QLayout *contentLayout() const { return m_layout; }
    WConfigItemWidget *memberWidget(const QString &key) const;
    // Framework-internal: callback invoked after each member is created (applies item adjusters).
    void setItemHook(
        std::function<void(WConfigItemWidget *, const QString &)> hook);

signals:
    void valueChanged();
    void clicked(we::config::WConfigItemWidget *widget);

private:
    WConfigViewer *m_viewer = nullptr;
    WConfigGroupSpec *m_spec = nullptr;
    QWidget *m_memberHost = nullptr;
    QLayout *m_layout = nullptr;
    QLabel *m_titleLabel = nullptr;
    QFrame *m_guide = nullptr;
    QHash<QString, WConfigItemWidget *> m_members;
    QHash<QString, bool> m_visible;
    std::function<void(WConfigItemWidget *, const QString &)> m_itemHook;
    void rebuildRows();
    void clearMembers();
};

} // namespace we::config

#endif // WCONFIGGROUPWIDGET_H