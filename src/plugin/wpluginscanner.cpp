/**
 * @file wpluginscanner.cpp
 * @brief Implementation of the plugin scanner.
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
#include "WECore/plugin/wpluginscanner.h"

#include "WECore/plugin/wplugin.h"
#include "WECore/plugin/wpluginconfigmanager.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/utils/wpath.h"
#include "WECore/we/we.h"
#include "WECore/we/webase.h"

#include <QFileInfo>
#include <QJsonDocument>

namespace we {

using namespace Consts;

WPluginScanner::WPluginScanner(const QString &configPath)
    : m_configManager(new WPluginConfigManager),
      m_configPath(configPath.isEmpty() ? QString::fromLatin1(Plugins::ConfigPath)
                                        : configPath) {}

WPluginScanner::~WPluginScanner() { delete m_configManager; }

QJsonObject WPluginScanner::readJsonFile(QString filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QJsonObject();
    const QString content = file.readAll();
    file.close();
    return QJsonDocument::fromJson(content.toUtf8()).object();
}

void WPluginScanner::findAllPlugin() {
    const QString pluginsPath = WPath().getModuleFolder() + m_configPath;
    if (!m_configManager->loadFromFile(pluginsPath))
        return;

    WPluginTreeNode *root = m_configManager->rootNode();
    std::function<void(WPluginTreeNode *)> loadNode =
        [&](WPluginTreeNode *node) {
            if (!node->isContainer) {
                const QFileInfo fileInfo(node->absolutePath);
                const QJsonObject pluginObj = readJsonFile(node->absolutePath);
                if (!pluginObj.isEmpty())
                    findSinglePlugin(QDir(fileInfo.filePath()), pluginObj);
            }
            for (WPluginTreeNode *child : std::as_const(node->children))
                loadNode(child);
        };

    for (WPluginTreeNode *child : std::as_const(root->children))
        loadNode(child);
}

void WPluginScanner::findPluginRecursive(QDir curPath, QJsonObject object) {
    const QDir filePath = WPath().resolvePath(
        curPath.path(), object[Plugins::PluginConfigPath].toString());

    const QJsonObject jsonObj = readJsonFile(filePath.path());
    if (jsonObj.isEmpty())
        return;

    if (jsonObj.contains(Plugins::Plugins)) {
        const QJsonObject pluginsObj = jsonObj[Plugins::Plugins].toObject();
        for (auto it = pluginsObj.begin(); it != pluginsObj.end(); ++it)
            findPluginRecursive(filePath, it.value().toObject());
    } else {
        findSinglePlugin(filePath, jsonObj);
    }
}

void WPluginScanner::findPlugin(QDir curPath, QJsonObject object) {
    const QDir pluginPath = WPath().resolvePath(
        curPath.path(), object[Plugins::PluginConfigPath].toString());
    const QJsonObject pluginObj = readJsonFile(pluginPath.path());
    if (pluginObj.isEmpty())
        return;
    findSinglePlugin(curPath, pluginObj);
}

WPluginConfigManager *WPluginScanner::pluginConfigManager() {
    return m_configManager;
}

void WPluginScanner::findSinglePlugin(QDir curPath, QJsonObject pluginObj) {
    if (!pluginObj.contains(Plugin::RelativePath))
        return;

    WPlugin *plugin = new WPlugin(WE::inst()->getWEClass()->pluginManager());
    plugin->readConfig(curPath.path(), pluginObj);
    WE::inst()->getWEClass()->pluginManager()->addPlugin(plugin);
}

} // namespace we