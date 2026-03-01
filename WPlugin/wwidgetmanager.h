/**
 * @file wwidgetmanager.h
 * @brief Widget管理器头文件，负责插件Widget的统一管理
 * @author howdy213
 * @date 2026-1-31
 * @version 1.2.0
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
#ifndef WWIDGETMANAGER_H
#define WWIDGETMANAGER_H
#include <QMap>
#include <QObject>
#include <QUuid>

#include "../WDef/wedef.h"
#include "../WE/webase.h"
#include "wwidget.h"

class WWidgetManagerPrivate;
W_INLINE namespace WE_NAMESPACE {
    class WE_EXPORT WWidgetManager : public QObject {
    Q_OBJECT
    public:
    WWidgetManager(WEBase *base = nullptr);
    virtual ~WWidgetManager();
    bool addWidget(QUuid id, WWidget *widget);
    WWidget *getWidget(QUuid id);
    QVector<WWidget *> getWidgets();
    QVector<WWidget *> getWidget(QString key, QVariant value);
    QUuid getUuid(WWidget *widget);
    QVariant changeVariant(QString key, QVariant value);
    void initWidget();

private:
    WWidgetManagerPrivate *d = nullptr;
    };
}
Q_DECLARE_METATYPE(WE_NAMESPACE::WWidgetManager)
Q_DECLARE_METATYPE(WE_NAMESPACE::WWidgetManager *)
#endif // WWIDGETMANAGER_H
