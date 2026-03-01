/**
 * @file wwidget.h
 * @brief WWidget类头文件，定义插件Widget的基类
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
#ifndef WWIDGET_H
#define WWIDGET_H
#include <QObject>

#include "../WE/webase.h"
#include "../WPlugin/wpluginmetadata.h"

class WWidgetPrivate;
W_INLINE namespace WE_NAMESPACE {
    class WE_EXPORT WWidget : public QObject {
    Q_OBJECT
    signals:
    void sendMessage(WMetaData &);
    private slots:
    virtual void receiveMessage(WMetaData &msg) = 0;

    public:
    WWidget(WEBase *parent = nullptr);
    virtual ~WWidget();
    bool setParent(WEBase *parent);
    WEBase *getParent();
    void sendMessageCallback(WMetaData &data);
    void receiveMessageCallback(WMetaData &msg);
    virtual void initWidget() = 0;
    virtual bool connectMessage(bool isConnect = true);
    QVariant getAttr(QString key);
    bool setAttr(QString key, QVariant value);

private:
    WWidgetPrivate *d = nullptr;
    };
}
#endif // WWIDGET_H
