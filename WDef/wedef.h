/**
 * @file wedef.h
 * @brief 核心定义头文件，包含类的前置声明
 * @author howdy213
 * @date 2026-01-30
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
#ifndef WEDEF_H
#define WEDEF_H

#define EX(a) a
#define _LS(a, b) a##b
#define LS(a, b) _LS(a, b)
#define _MS(a) #a
#define MS(a) _MS(a)
#define V2V(type, key) qvariant_cast<type>(var[key])

#define _WE_VERSION 1_1_0
#define WE_VERSION _WE_VERSION
#define _WE_VERSION_STRING 1.1.0
#define WE_VERSION_STRING MS(_WE_VERSION_STRING)

#define WE_NAMESPACE we
#if defined(WE_LIBRARY)
#define WE_EXPORT Q_DECL_EXPORT
#define WE_EXPORT2 Q_DECL_EXPORT
#else
#define WE_EXPORT Q_DECL_IMPORT
#define WE_EXPORT2
#endif
//TODO：修正对含Q_OBJECT导出类的处理，使用WE_EXPORT2，会导致类多次拷贝影响兼容性
//已修正：不在WECore.pri中包含以避免生成moc
#define W_INLINE inline
#define WApp (WE::inst())

#include <QtGlobal>
W_INLINE namespace WE_NAMESPACE {
namespace Consts {
constexpr auto WESender = "WidgetExplorer";
namespace PluginInterface {
constexpr auto IID = "QPlugins.WPluginManager.WPluginInterface";
}
namespace Config {
constexpr auto ConfigPath = "config/config.json";
constexpr auto Scale = "Scale";             // string(x.x)
constexpr auto Font = "Font";               // string(int)
constexpr auto DefaultMain = "DefaultMain"; // string(path)
} // namespace Config
namespace Plugins {
constexpr auto PluginConfigPath = "Path";
constexpr auto ConfigPath = "plugins/plugins.json";
constexpr auto ConfigFolder = "plugins/";
} // namespace Plugins
namespace Data {
constexpr auto Plugin = "Plugin";   // WPlugin*
constexpr auto WEBase = "WE";       // WEBase*
constexpr auto Params = "Params";   // string(string list)
constexpr auto Command = "Command"; // string
} // namespace Data
namespace Plugin {
constexpr auto Version = "Version";       // string
constexpr auto Name = "Name";             // string
constexpr auto Init = "Init";             // string(string list)
constexpr auto Path = "Path";             // string(path)
constexpr auto Date = "Date";             // string
constexpr auto Author = "Author";         // string
constexpr auto Desc = "Desc";             // string
constexpr auto Type = "Type";             // string
constexpr auto MainWidget = "MainWidget"; // bool
constexpr auto Autorun = "Autorun";       // bool
constexpr auto Admin = "Admin";           // bool
} // namespace Plugin
namespace Widget {
constexpr auto Name = "Name"; // string
constexpr auto Desc = "Desc"; // string
} // namespace Widget
namespace Public {
constexpr auto PManager = "WPluginManager.pluginManager"; // WPluginManager*
constexpr auto WManager = "WWidgetManager.widgetManager"; // WWidgetManager*
constexpr auto Config = "WConfigDocument.configManager";  // WConfigManager*
constexpr auto Path = "WPath.pathManager";                // WPath*
} // namespace Public
} // namespace Consts
}; // namespace WE_NAMESPACE

W_INLINE namespace WE_NAMESPACE {
class WE;
class WEBase;
class WEBaseData;
class LightWidget;
class LightMain;
class LightSystem;
class WidgetExplorer;
class WPlugin;
class WidgetDebugger;
class WPath;
class WPluginManager;
class WConfigDocument;
class WWidgetManager;
class WWidget;
class WEClass;
class WVirtualPlugin;
class WPluginProxy;
template <class T> class WConfig;
} // namespace WE_NAMESPACE

template <typename funcType> void *getMemberAddr(funcType func) {
    union {
        void *pv;
        funcType pfn;
    } u;
    u.pfn = func;
    return u.pv;
}//测试类成员偏移量

#endif // WEDEF_H
