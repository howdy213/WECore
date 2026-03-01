/**
 * @file wwidgetmanager.cpp
 * @brief Widget管理器实现文件
 * @author howdy213
 * @date 2026-1-30
 * @version 1.1.0
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
#include <QRandomGenerator>

#include "wwidgetmanager.h"
using namespace we::Consts;

///
/// \brief The WWidgetManagerPrivate class
///
class WWidgetManagerPrivate {
public:
    QMap<QUuid, WWidget *> widgets;
};
///
/// \brief WWidgetManager::WWidgetManager
/// \param base
///
WWidgetManager::WWidgetManager(WEBase *base) : QObject() {
    Q_UNUSED(base)
    d = new WWidgetManagerPrivate;
}
///
/// \brief WWidgetManager::~WWidgetManager
///
WWidgetManager::~WWidgetManager() {
    delete d;
    d = nullptr;
}
///
/// \brief WWidgetManager::addWidget
/// \param id
/// \param widget
/// \return
///
bool WWidgetManager::addWidget(QUuid id, WWidget *widget) {
    if (widget == nullptr)
        return 0;
    if (d->widgets.contains(id))
        return 0;
    if (d->widgets.values().contains(widget))
        return 0;
    if (qvariant_cast<QString>(widget->getAttr(Widget::Name)) == "")
        widget->setAttr(
            Widget::Name,
            "Widget-" + QString::number(
                (long long int)QRandomGenerator::system()->generate()));
    d->widgets.insert(id, widget);
    return 1;
}
///
/// \brief WWidgetManager::getWidget
/// \param id
/// \return
///
WWidget *WWidgetManager::getWidget(QUuid id) {
    auto res = d->widgets.find(id);
    return res == d->widgets.end() ? nullptr : res.value();
}

QVector<WWidget *> WWidgetManager::getWidgets() { return d->widgets.values(); }
///
/// \brief WWidgetManager::getWidget
/// \param key
/// \param value
/// \return
///
QVector<WWidget *> WWidgetManager::getWidget(QString key, QVariant value) {
    QVector<WWidget *> res;
    for (auto it = d->widgets.begin(); it != d->widgets.end(); it++) {
        if (it.value()->getAttr(key) == value)
            res += it.value();
    }
    return res;
}
///
/// \brief WWidgetManager::getUuid
/// \param widget
/// \return
///
QUuid WWidgetManager::getUuid(WWidget *widget) {
    for (auto it = d->widgets.begin(); it != d->widgets.end(); it++) {
        if (it.value() == widget)
            return it.key();
    }
    return QUuid();
}
///
/// \brief WWidgetManager::changeVariant
/// \param key
/// \param value
/// \return
///
QVariant WWidgetManager::changeVariant(QString key, QVariant value) {
    if (key == Widget::Name) {
        QString str = qvariant_cast<QString>(value);
        auto widget=getWidget(Widget::Name, str);
        if (!widget.empty())
            return str + "#";
    }
    return value;
}
///
/// \brief WWidgetManager::initWidget
///
void WWidgetManager::initWidget() {
    auto widget = d->widgets.begin();
    for (; widget != d->widgets.end(); widget++) {
        widget.value()->connectMessage();
        widget.value()->initWidget();
    }
}
