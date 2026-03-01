/**
 * @file wwidget.cpp
 * @brief WWidget类实现文件
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
#include "wwidget.h"
#include "../WPlugin/wwidgetmanager.h"

///
/// \brief The WWidgetPrivate class
///
class WWidgetPrivate {
public:
    WEBase *parent = nullptr;
    QMap<QString, QVariant> attr;
};
///
/// \brief WWidget::WWidget
/// \param parent
///
WWidget::WWidget(WEBase *parent) {
    d = new WWidgetPrivate;
    this->d->parent = parent;
}
///
/// \brief WWidget::~WWidget
///
WWidget::~WWidget() {
    delete d;
    d = nullptr;
}
///
/// \brief WWidget::sendMessageCallback
/// \param data
///
void WWidget::sendMessageCallback(WMetaData &data) { emit sendMessage(data); }
///
/// \brief WWidget::setParent
/// \param parent
/// \return
///
bool WWidget::setParent(WEBase *parent) {
    this->d->parent = parent;
    return true;
}
///
/// \brief WWidget::getParent
/// \return
///
WEBase *WWidget::getParent() { return this->d->parent; }
///
/// \brief WWidget::receiveMessageCallback
/// \param msg
///
void WWidget::receiveMessageCallback(WMetaData &msg) { receiveMessage(msg); }
///
/// \brief WWidget::connectMessage
/// \param isConnect
/// \return
///
bool WWidget::connectMessage(bool isConnect) {
    return isConnect ? connect(this, &WWidget::sendMessage, this,
                               &WWidget::receiveMessageCallback)
                     : disconnect(this, &WWidget::sendMessage, this,
                                  &WWidget::receiveMessageCallback);
}
///
/// \brief WWidget::getAttr
/// \param key
/// \return
///
QVariant WWidget::getAttr(QString key) {
    auto it = this->d->attr.find(key);
    return it == d->attr.end() ? QVariant() : it.value();
}
///
/// \brief WWidget::setAttr
/// \param key
/// \param value
/// \return
///
bool WWidget::setAttr(QString key, QVariant value) {
    QVariant var;
    if (d->parent)
        var = d->parent->getWEClass()->widgetManager()->changeVariant(key, value);
    this->d->attr.insert(key, var);
    return true;
}
