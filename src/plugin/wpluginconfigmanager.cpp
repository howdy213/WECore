/**
 * @file wpluginconfigmanager.cpp
 * @brief Implementation of the plugin configuration tree.
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
#include "WECore/plugin/wpluginconfigmanager.h"

#include "WECore/utils/wpath.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>

namespace we {

using namespace Consts;

WPluginConfigManager::WPluginConfigManager(QObject *parent) : QObject(parent) {
    rootNode_ = new WPluginTreeNode;
    rootNode_->isContainer = true;
}

WPluginConfigManager::~WPluginConfigManager() {
    clearTree();
    delete rootNode_;
}

void WPluginConfigManager::clearTree() {
    std::function<void(WPluginTreeNode *)> deleteChildren =
        [&](WPluginTreeNode *node) {
            for (WPluginTreeNode *child : std::as_const(node->children)) {
                deleteChildren(child);
                delete child;
            }
            node->children.clear();
        };
    deleteChildren(rootNode_);
}

bool WPluginConfigManager::loadFromFile(const QString &rootJsonPath) {
    clearTree();
    rootJsonPath_ = rootJsonPath;
    rootNode_->absolutePath = rootJsonPath_;

    bool ok = false;
    const QJsonObject rootObj = readJsonFromFile(rootJsonPath_, &ok);
    if (!ok)
        return false;

    const QJsonValue pluginsVal = rootObj.value(Plugins::Plugins);
    if (!pluginsVal.isObject())
        return false;

    const QJsonObject plugins = pluginsVal.toObject();
    const QString baseDir = QFileInfo(rootJsonPath_).absolutePath();

    for (const QString &key : plugins.keys()) {
        const QJsonObject val = plugins.value(key).toObject();
        const QString path = val.value(Plugins::PluginConfigPath).toString();
        buildNodeRecursive(key, path, rootNode_, baseDir);
    }
    return true;
}

bool WPluginConfigManager::refresh() {
    if (rootJsonPath_.isEmpty())
        return false;
    return loadFromFile(rootJsonPath_);
}

void WPluginConfigManager::buildNodeRecursive(const QString &key,
                                              const QString &path,
                                              WPluginTreeNode *parent,
                                              const QString &baseDir) {
    auto *node = new WPluginTreeNode;
    node->key = key;
    node->path = path;
    node->absolutePath = resolvePath(baseDir, path);
    node->parent = parent;
    parent->children.append(node);

    bool ok = false;
    const QJsonObject childObj = readJsonFromFile(node->absolutePath, &ok);
    if (ok && childObj.value(Plugins::Plugins).isObject()) {
        node->isContainer = true;
        const QJsonObject childPlugins =
            childObj.value(Plugins::Plugins).toObject();
        const QString childBaseDir =
            QFileInfo(node->absolutePath).absolutePath();
        for (const QString &childKey : childPlugins.keys()) {
            const QJsonObject childVal = childPlugins.value(childKey).toObject();
            const QString childPath =
                childVal.value(Plugins::PluginConfigPath).toString();
            buildNodeRecursive(childKey, childPath, node, childBaseDir);
        }
    } else {
        node->isContainer = false;
    }
}

QString WPluginConfigManager::resolvePath(const QString &baseDir,
                                          const QString &path) const {
    WPath wpath;
    return wpath.resolvePath(baseDir, path);
}

QJsonObject WPluginConfigManager::readJsonFromFile(const QString &filePath,
                                                   bool *ok) const {
    if (ok)
        *ok = false;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QJsonObject();

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return QJsonObject();

    if (ok)
        *ok = true;
    return doc.object();
}

bool WPluginConfigManager::writeJsonToFile(const QString &filePath,
                                           const QJsonObject &obj) const {
    const QFileInfo fi(filePath);
    QDir().mkpath(fi.absolutePath());
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    return true;
}

bool WPluginConfigManager::addNode(WPluginTreeNode *parent, const QString &key,
                                   const QString &path) {
    if (key.isEmpty() || path.isEmpty())
        return false;

    const QString parentJsonPath =
        (parent == nullptr || parent == rootNode_) ? rootJsonPath_
                                                   : parent->absolutePath;

    bool ok = false;
    QJsonObject parentObj = readJsonFromFile(parentJsonPath, &ok);
    if (!ok)
        return false;

    QJsonObject plugins = parentObj.value(Plugins::Plugins).toObject();
    if (plugins.contains(key))
        return false; // A sibling entry with the same KeyName exists.

    QJsonObject newObj;
    newObj[Plugins::PluginConfigPath] = path;
    plugins.insert(key, newObj);
    parentObj[Plugins::Plugins] = plugins;

    if (!writeJsonToFile(parentJsonPath, parentObj))
        return false;
    return refresh();
}

bool WPluginConfigManager::removeNode(WPluginTreeNode *node) {
    if (!node || node == rootNode_)
        return false;

    const QString parentJsonPath =
        (node->parent == nullptr || node->parent == rootNode_)
            ? rootJsonPath_
            : node->parent->absolutePath;

    bool ok = false;
    QJsonObject parentObj = readJsonFromFile(parentJsonPath, &ok);
    if (!ok)
        return false;

    QJsonObject plugins = parentObj.value(Plugins::Plugins).toObject();
    if (!plugins.contains(node->key))
        return false;

    plugins.remove(node->key);
    parentObj[Plugins::Plugins] = plugins;

    if (!writeJsonToFile(parentJsonPath, parentObj))
        return false;
    return refresh();
}

bool WPluginConfigManager::setNodePath(WPluginTreeNode *node,
                                       const QString &newPath) {
    if (!node || node == rootNode_ || newPath.isEmpty())
        return false;

    const QString parentJsonPath =
        (node->parent == nullptr || node->parent == rootNode_)
            ? rootJsonPath_
            : node->parent->absolutePath;

    bool ok = false;
    QJsonObject parentObj = readJsonFromFile(parentJsonPath, &ok);
    if (!ok)
        return false;

    QJsonObject plugins = parentObj.value(Plugins::Plugins).toObject();
    QJsonObject target = plugins.value(node->key).toObject();
    if (target.isEmpty())
        return false;

    target[Plugins::PluginConfigPath] = newPath;
    plugins[node->key] = target;
    parentObj[Plugins::Plugins] = plugins;

    if (!writeJsonToFile(parentJsonPath, parentObj))
        return false;
    return refresh();
}

QString WPluginConfigManager::readFileContent(WPluginTreeNode *node) const {
    if (!node)
        return QString();
    QFile file(node->absolutePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromUtf8(file.readAll());
}

void WPluginConfigManager::traverse(
    const std::function<void(WPluginTreeNode *)> &func) const {
    std::function<void(WPluginTreeNode *)> dfs = [&](WPluginTreeNode *node) {
        func(node);
        for (WPluginTreeNode *child : std::as_const(node->children))
            dfs(child);
    };
    for (WPluginTreeNode *child : std::as_const(rootNode_->children))
        dfs(child);
}

} // namespace we