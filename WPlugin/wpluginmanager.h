/**
 * @file wpluginmanager.h
 * @brief 插件管理器头文件
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
#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H
#include <QObject>
#include <QPluginLoader>
#include <QVariant>

#include "../WDef/wedef.h"
#include "../WPlugin/wpluginmetadata.h"

using InitDataProc = std::function<void(WMetaData &)>;
class WPluginManagerPrivate;
W_INLINE namespace WE_NAMESPACE {
    class WE_EXPORT WPluginManager : public QObject {
    Q_OBJECT
    public:
    WPluginManager();
    virtual ~WPluginManager();

    bool addPlugin(WPlugin *plugin);
    bool loadPlugin(WPlugin *plugin);
    bool initPlugin(WPlugin *plugin, InitDataProc proc = [](WMetaData &) {});
    bool unloadPlugin(WPlugin *plugin);
    void unloadAllPlugins();

    QUuid getPluginByName(QString name);
    WPlugin *getPluginById(QUuid id);
    QVector<WPlugin *> allPluginsInst();
    QVariant setPluginData(QUuid id, QString key, QVariant value);

    QVector<QUuid> getPluginByAttr(QString key, QVariant value);
    QVector<QUuid> allPluginsId();
    QUuid getUuid(WPlugin *plugin);

private:
    WPlugin *createPlugin(QString configPath, QString config);
    bool unloadPlugin(QMap<QUuid, WPlugin *>::Iterator it);
    WPluginManagerPrivate *d = nullptr;
public slots:
    bool sendMsg(WMetaData &);
    };
}
Q_DECLARE_METATYPE(WE_NAMESPACE::WPluginManager)
Q_DECLARE_METATYPE(WE_NAMESPACE::WPluginManager *)
#endif
