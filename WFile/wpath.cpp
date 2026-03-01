/**
 * @file wpath.cpp
 * @brief 路径处理实现文件
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
#include "wpath.h"
#include "../WPlugin/wplugin.h"
#include "../WE/webase.h"

#include <QDir>
using namespace we::Consts;

///
/// \brief The WPathPrivate class
///
class WPathPrivate {
public:
    QString moduleName = "";
    WEBase *we = nullptr;
};

///
/// \brief WPath::WPath
/// \param we
///
WPath::WPath(WEBase *we) {
    d = new WPathPrivate;
    d->we = we;
}
///
/// \brief WPath::~WPath
///
WPath::~WPath() {
    delete d;
    d = nullptr;
}
///
/// \brief WPath::setWEBase
/// \param base
///
void WPath::setWEBase(WEBase *base) { d->we = base; }
///
/// \brief WPath::getModulePath
/// \return
///
QString WPath::getModulePath() {
    WCHAR p[MAX_PATH];
    GetModuleFileName(NULL, p, MAX_PATH);
    QString path = QString::fromWCharArray(p);
    return QDir::cleanPath(path);
}
///
/// \brief WPath::getModuleFolder
/// \return
///
QString WPath::getModuleFolder() { return splitPath(getModulePath()); }
///
/// \brief WPath::getModulePath
/// \param moduleName
/// \return
///
QString WPath::getModulePath(QUuid moduleId) {
    if (d->we == nullptr)
        return "";
    auto pm = d->we->getWEClass()->pluginManager();
    auto plugin = pm->getPluginById(moduleId);
    return qvariant_cast<QString>(plugin->getMetaData(Plugin::Path));
}
///
/// \brief WPath::getModuleFolder
/// \param moduleName
/// \return
///
QString WPath::getModuleFolder(QUuid moduleId) {
    return splitPath(getModulePath(moduleId));
}
///
/// \brief WPath::getModulePath
/// \param plugin
/// \return
///
QString WPath::getModulePath(WPlugin *plugin) {
    if (d->we == nullptr)
        return "";
    return qvariant_cast<QString>(plugin->getMetaData(Plugin::Path));
}
///
/// \brief WPath::getModuleFolder
/// \param plugin
/// \return
///
QString WPath::getModuleFolder(WPlugin *plugin) {
    return splitPath(getModulePath(plugin));
}
///
/// \brief WPath::splitPath
/// \param path
/// \return
///
QString WPath::splitPath(QString path) {
    path = QDir::cleanPath(path);
    return path.mid(0, path.lastIndexOf("/") + 1);
}
///
/// \brief WPath::transPath
/// \param cur
/// \param des
/// \return
///
QString WPath::transPath(QString cur, QString des) {
    cur = QDir::cleanPath(cur);
    des = QDir::cleanPath(des);
    QFileInfo info(cur);

    if (info.isFile()) {
        cur = splitPath(cur);
    }

    QDir currentDir(cur);

    if (QDir::isAbsolutePath(des))
        return QDir::cleanPath(des);
    QString absolutePath = currentDir.absoluteFilePath(des);
    absolutePath = QDir::cleanPath(absolutePath);
    return absolutePath;
}
