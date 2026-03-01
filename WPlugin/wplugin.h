/**
 * @file wplugin.h
 * @brief 插件基类头文件
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
#ifndef WPLUGIN_H
#define WPLUGIN_H
#include <QPluginLoader>

#include "../WDef/wedef.h"
#include "../WPlugin/wplugininterface.h"

class WPluginPrivate;

W_INLINE namespace WE_NAMESPACE{
class WE_EXPORT WPlugin{
public:
    WPlugin(WPluginManager *parent);
    virtual ~WPlugin();
    bool readConfig(QString filePath, QString config);
    bool load();
    bool unload();
    bool available();
    QVariant getMetaData(QString key);
    bool hasMetaData(QString key);
    QUuid getId();
    void setMetaData(QString key, QVariant value);
    WPluginInterface *inst();

private:
    bool loadDll(QString dllPath);
    bool loadExe(QString ExePath);
    bool loadFile(QString filePath);
    WPluginPrivate *d = nullptr;
};
}
Q_DECLARE_METATYPE(WE_NAMESPACE::WPlugin);
Q_DECLARE_METATYPE(WE_NAMESPACE::WPlugin *);

#endif // WPLUGIN_H
