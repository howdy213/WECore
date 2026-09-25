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
#include "WECore/config/WConfigGroupWidget.h"
#include "WECore/config/WConfigItemWidget.h"
#include "WECore/config/WConfigLayout.h"
#include <QBoxLayout>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <utility>

namespace we::config {

WConfigGroupWidget::WConfigGroupWidget(WConfigViewer *viewer,
                                       WConfigGroupSpec *spec, QWidget *parent)
    : QWidget(parent), m_viewer(viewer), m_spec(spec) {
    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(Layout::ContentsMargin, 0,
                              Layout::ContentsMargin, 0);
    outer->setSpacing(Layout::RowSpacing);

    // No displayName means an untitled group: no title shown, used only to host the member layout.
    const bool hasTitle = m_spec && !m_spec->displayName.isEmpty();
    m_titleLabel = new QLabel(hasTitle ? m_spec->displayName : QString(), this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setObjectName("GroupTitleLabel");
    m_titleLabel->setWordWrap(true); // Wrap long titles to avoid horizontal overflow
    m_titleLabel->setVisible(hasTitle);
    if (hasTitle && !m_spec->description.isEmpty())
        m_titleLabel->setToolTip(m_spec->description);
    outer->addWidget(m_titleLabel);

    // Member area: a vertical guide aligned with the group title's left edge, with a gap
    // before the members, to convey the group hierarchy.
    QHBoxLayout *body = new QHBoxLayout();
    body->setContentsMargins(0, 0, 0, 0);
    body->setSpacing(0);

    m_guide = new QFrame(this);
    m_guide->setFrameShape(QFrame::VLine);
    m_guide->setFrameShadow(QFrame::Plain);
    m_guide->setFixedWidth(1);
    m_guide->setVisible(hasTitle);
    body->addWidget(m_guide);
    body->addSpacing(hasTitle ? Layout::GroupGuideGap : 0);

    m_memberHost = new QWidget(this);
    body->addWidget(m_memberHost, 1);
    outer->addLayout(body);

    rebuildRows();
}

void WConfigGroupWidget::clearMembers() {
    // Take items out of the layout without deleting the widgets, then delete the
    // widgets separately -- avoids a double free.
    if (m_layout) {
        while (QLayoutItem *child = m_layout->takeAt(0))
            delete child;
    }
    for (WConfigItemWidget *item : std::as_const(m_members))
        delete item;
    m_members.clear();
}

void WConfigGroupWidget::rebuildRows() {
    clearMembers();
    // Members are gone; uninstall the old layout so QWidget accepts a new one.
    if (m_layout) {
        delete m_layout;
        m_layout = nullptr;
    }

    if (!m_spec)
        return;

    // 1) Create all member widgets first (not yet in a layout) so layoutBuilder can
    //    fetch them via memberWidget(key) and arrange them itself.
    QList<WConfigItemWidget *> created;
    for (const auto &member : m_spec->members) {
        const QString &key = member.first;
        if (!m_visible.value(key, true))
            continue;
        WConfigDataBase *data = m_viewer ? m_viewer->getConfigData(key) : nullptr;
        if (!data)
            continue;

        WConfigItemMode mode = member.second;
        if (mode == WConfigItemMode::Automatic) {
            switch (data->type()) {
            case DataType::Array:
            case DataType::Object:
                mode = WConfigItemMode::Full;
                break;
            case DataType::Action:
                mode = WConfigItemMode::ButtonOnly;
                break;
            default:
                mode = WConfigItemMode::Inline;
                break;
            }
        }

        // Use the framework's item styles: Inline keeps per-item titles (aligned within
        // the group), Full for Array/Object, ButtonOnly maps to Compact (button only).
        WConfigItemWidgetStyle style = WConfigItemWidgetStyle::Compact;
        switch (mode) {
        case WConfigItemMode::Inline:
            style = WConfigItemWidgetStyle::Inline;
            break;
        case WConfigItemMode::Full:
            style = WConfigItemWidgetStyle::Full;
            break;
        case WConfigItemMode::ButtonOnly:
            style = WConfigItemWidgetStyle::Compact;
            break;
        case WConfigItemMode::Hidden:
        default:
            continue;
        }

        auto *item = new WConfigItemWidget(data, style, m_memberHost);
        item->setShowDescription(false);
        item->setStyleTooltip();
        // Zero the item's horizontal padding so member titles align with the group title's left edge.
        if (QLayout *inner = item->layout()) {
            inner->setContentsMargins(0, Layout::ItemSpacing, 0,
                                      Layout::ItemSpacing);
        }
        m_members.insert(key, item);
        created.append(item);

        connect(item, &WConfigItemWidget::valueChanged, this,
                &WConfigGroupWidget::valueChanged);
        connect(item, &WConfigItemWidget::clicked, this,
                &WConfigGroupWidget::clicked);

        if (m_itemHook)
            m_itemHook(item, key);
    }

    // 2) Member-area layout: prefer the layoutBuilder supplied at creation time.
    QLayout *layout =
        m_spec->layoutBuilder ? m_spec->layoutBuilder(this) : nullptr;
    const bool custom = (layout != nullptr);
    if (!custom) {
        auto *box = new QVBoxLayout;
        box->setContentsMargins(0, 0, 0, 0);
        box->setSpacing(Layout::ItemSpacing);
        layout = box;
    }

    m_layout = layout;
    m_memberHost->setLayout(m_layout);

    // 3) Append members the builder did not adopt, so none is lost.
    for (WConfigItemWidget *item : std::as_const(created)) {
        if (m_layout->indexOf(item) < 0)
            m_layout->addWidget(item);
        item->show();
    }

    // Default box layout adds a stretch (pushes content up); custom layouts decide for themselves.
    if (!custom) {
        if (auto *box = qobject_cast<QBoxLayout *>(m_layout))
            box->addStretch();
    }
}

void WConfigGroupWidget::applySpec() { rebuildRows(); }

void WConfigGroupWidget::setItemMode(const QString &key,
                                     WConfigItemMode mode) {
    if (!m_spec)
        return;
    for (auto &member : m_spec->members) {
        if (member.first == key) {
            member.second = mode;
            rebuildRows();
            return;
        }
    }
}

void WConfigGroupWidget::setItemVisible(const QString &key, bool on) {
    m_visible.insert(key, on);
    rebuildRows();
}

void WConfigGroupWidget::setGroupDescription(const QString &desc) {
    if (m_spec) {
        m_spec->description = desc;
        m_titleLabel->setToolTip(desc);
    }
}

void WConfigGroupWidget::setTitleVisible(bool on) { m_titleLabel->setVisible(on); }

WConfigItemWidget *WConfigGroupWidget::memberWidget(const QString &key) const {
    return m_members.value(key, nullptr);
}

void WConfigGroupWidget::setItemHook(
    std::function<void(WConfigItemWidget *, const QString &)> hook) {
    m_itemHook = std::move(hook);
}

QList<WConfigItemWidget *> WConfigGroupWidget::memberWidgets() const {
    return m_members.values();
}

void WConfigGroupWidget::currentValue() const {
    for (WConfigItemWidget *w : std::as_const(m_members))
        w->currentValue();
}

void WConfigGroupWidget::refresh() {
    for (WConfigItemWidget *w : std::as_const(m_members))
        w->refresh();
}

} // namespace we::config