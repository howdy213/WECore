/**
 * @file wpluginscanner.h
 * @brief Discovers plugins by walking the plugin configuration tree.
 *
 * WPluginScanner loads the plugin list through WPluginConfigManager, then
 * instantiates and registers every plugin whose configuration is reached, in
 * depth-first order.
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
#ifndef WPLUGINSCANNER_H
#define WPLUGINSCANNER_H

#include <QDir>
#include <QJsonObject>
#include <QString>

#include "WECore/def/wedef.h"

namespace we {

class WPluginConfigManager;

/**
 * @brief Walks the plugin configuration tree and loads every plugin found.
 *
 * The entry file defaults to Consts::Plugins::ConfigPath resolved against the
 * module folder; pass another relative path to scan a different list.
 */
class WE_EXPORT WPluginScanner {
public:
    explicit WPluginScanner(const QString &configPath = QString());
    virtual ~WPluginScanner();

    /// Scans the whole tree, registering every plugin it reaches.
    void findAllPlugin();
    /// Registers the single plugin described by @p pluginObj.
    void findSinglePlugin(QDir curPath, QJsonObject pluginObj);
    void findPluginRecursive(QDir rootPath, QJsonObject object);
    void findPlugin(QDir curPath, QJsonObject object);

    /// The configuration tree used by the last scan; owned by the scanner.
    WPluginConfigManager *pluginConfigManager();

private:
    static QJsonObject readJsonFile(QString filePath);

    WPluginConfigManager *m_configManager = nullptr;
    QString m_configPath;
};

} // namespace we

#endif // WPLUGINSCANNER_H