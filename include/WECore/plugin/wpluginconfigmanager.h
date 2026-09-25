/**
 * @file wpluginconfigmanager.h
 * @brief Plugin configuration tree: loads the plugin list into a node tree and
 * writes edits back to disk.
 *
 * WPluginConfigManager models the nested plugin configuration files as a tree
 * of WPluginTreeNode. Container nodes are files that list child plugins under
 * a "Plugins" object; leaf nodes are plugin metadata files.
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
#ifndef WPLUGINCONFIGMANAGER_H
#define WPLUGINCONFIGMANAGER_H

#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>

#include <functional>

#include "WECore/def/wedef.h"

namespace we {

/**
 * @brief One entry of the plugin configuration tree.
 *
 * @c isContainer is set when the file the node points at holds a "Plugins"
 * object (i.e. it lists further entries). The tree, including its root, is
 * owned by the WPluginConfigManager that built it.
 */
struct WPluginTreeNode {
    QString key;              ///< KeyName inside the parent file.
    QString path;             ///< Path as written in the parent file.
    QString absolutePath;     ///< Resolved path.
    bool isContainer = false; ///< True when the file lists child plugins.
    WPluginTreeNode *parent = nullptr;
    QList<WPluginTreeNode *> children;
};

/**
 * @brief Reads, queries and edits the plugin configuration tree.
 */
class WE_EXPORT WPluginConfigManager : public QObject {
    Q_OBJECT

public:
    explicit WPluginConfigManager(QObject *parent = nullptr);
    ~WPluginConfigManager() override;

    /// Rebuilds the whole tree from @p rootJsonPath; false if it cannot be read.
    bool loadFromFile(const QString &rootJsonPath);
    /// Reloads the file passed to the last successful loadFromFile().
    bool refresh();

    /// Virtual root; its children are the top-level entries.
    WPluginTreeNode *rootNode() const { return rootNode_; }
    /// Top-level entries, i.e. the children of the virtual root.
    QList<WPluginTreeNode *> topLevelNodes() const {
        return rootNode_->children;
    }

    /**
   * @brief Adds an entry @p key pointing at @p path.
   * @param parent Target file; nullptr or rootNode() means the root file.
   */
    bool addNode(WPluginTreeNode *parent, const QString &key,
                 const QString &path);
    /// Removes @p node's entry from its parent file; the file itself is kept.
    bool removeNode(WPluginTreeNode *node);
    /// Rewrites @p node's Path entry in its parent file.
    bool setNodePath(WPluginTreeNode *node, const QString &newPath);

    /// Contents of the file @p node points at, for previewing.
    QString readFileContent(WPluginTreeNode *node) const;

    /// Depth-first traversal of every node below the virtual root.
    void traverse(const std::function<void(WPluginTreeNode *)> &func) const;

    /// The root JSON file the tree was loaded from.
    QString rootJsonPath() const { return rootJsonPath_; }

private:
    void clearTree();
    void buildNodeRecursive(const QString &key, const QString &path,
                            WPluginTreeNode *parent, const QString &baseDir);
    QJsonObject readJsonFromFile(const QString &filePath,
                                 bool *ok = nullptr) const;
    bool writeJsonToFile(const QString &filePath, const QJsonObject &obj) const;
    QString resolvePath(const QString &baseDir, const QString &path) const;

    WPluginTreeNode *rootNode_ = nullptr;
    QString rootJsonPath_;
};

} // namespace we

#endif // WPLUGINCONFIGMANAGER_H