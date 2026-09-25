/**
 * @file flowlayout.h
 * @brief Flow (wrapping) layout that reflows items to the available width.
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
#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H
#include <QLayout>
#include <QStyle>
#include <QWidget>

/**
 * @brief Arranges items along rows and wraps to the next row when the width runs out.
 *
 * The layout owns its items. Because the required height depends on the current
 * width, heightForWidth() is implemented; a parent QWidget only benefits from it
 * if its size policy has the heightForWidth flag set.
 */
class FlowLayout : public QLayout {
public:
    /// @p margin, @p hSpacing and @p vSpacing < 0 fall back to the parent's style metrics.
    FlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1,
               int vSpacing = -1);
    ~FlowLayout();
    void addItem(QLayoutItem *item) override;
    int horizontalSpacing() const;
    int verticalSpacing() const;

    /// Always empty: a flow layout does not expand in either direction.
    Qt::Orientations expandingDirections() const override;

    /// Always true: the required height is derived from the given width.
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;

    int count() const override;
    QLayoutItem *itemAt(int index) const override;
    QLayoutItem *takeAt(int index) override;
    void setGeometry(const QRect &rect) override;
    QSize sizeHint() const override;
    QSize minimumSize() const override;

private:
    /// Positions the items; with @p testOnly set, the geometry is only measured, not applied.
    int doLayout(const QRect &rect, bool testOnly) const;

    /// Reads a style pixel metric from the parent widget or parent layout.
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> m_items;
    int m_hSpace, m_vSpace;
};

#endif // FLOWLAYOUT_H
