/**
 * @file wedef.h
 * @brief Core definitions, constants, macros, and forward declarations.
 *
 * This header provides the central version string, export macros,
 * global helper macros, and the `we::Consts` namespace with all
 * well‑known key strings used across the framework. It also
 * forward‑declares every major class in the library.
 *
 * @author howdy213
 * @date 2026-05-01
 * @version 2.0.0
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

#include <QtGlobal>

#define EX(a) a
#define _LS(a, b) a##b
#define LS(a, b) _LS(a, b)
#define _MS(a) #a
#define MS(a) _MS(a)
#define _WE_VERSION 2.0.0
#define WE_VERSION MS(_WE_VERSION)

/// Export marker when building the library.
#if defined(WE_LIBRARY)
#define WE_EXPORT Q_DECL_EXPORT
#else
#define WE_EXPORT Q_DECL_IMPORT
#endif

/**
 * @brief Shortcut to retrieve the global WEBase instance.
 *
 * Equivalent to `we::WE::inst()`. Always returns a valid pointer
 * after the application has been initialised.
 */
#define WApp (WE::inst())

namespace we {

/**
 * @brief Well‑known constant strings used as keys, paths, etc.
 *
 * All constants are compile‑time `constexpr` values. They are
 * grouped into sub‑namespaces for clarity.
 */
namespace Consts {

constexpr auto WESender = "WidgetExplorer";

/**
 * @brief Plugin interface identifier.
 *
 * Must match the IID used with Q_DECLARE_INTERFACE and
 * Q_PLUGIN_METADATA.
 */
namespace PluginInterface {
constexpr auto IID = "QPlugins.WPluginManager.WPluginInterface";
} // namespace PluginInterface

/// Application‑level configuration keys.
namespace Config {
constexpr auto ConfigPath = "config/config.json";
constexpr auto Scale = "Scale";             ///< string(x.x)
constexpr auto Font = "Font";               ///< string(int)
constexpr auto DefaultMain = "DefaultMain"; ///< string(path)
} // namespace Config

/// Plugin discovery / configuration keys.
namespace Plugins {
constexpr auto PluginConfigPath = "Path";
constexpr auto ConfigPath = "plugins/plugins.json";
constexpr auto ConfigFolder = "plugins/";
constexpr auto Plugins = "Plugins";
} // namespace Plugins

/// Message / data map keys.
namespace Data {
constexpr auto Plugin = "Plugin"; ///< WPlugin*
constexpr auto WEBase = "WE";     ///< WEBase*
constexpr auto Params = "Params"; ///< string(string list)
constexpr auto Data = "Data"; ///< string(string list)
} // namespace Data

/// Plugin metadata keys.
namespace Plugin {
constexpr auto Version = "Version";       ///< string
constexpr auto Name = "Name";             ///< string
constexpr auto Init = "Init";             ///< string(string list)
constexpr auto Path = "Path";             ///< string(path)
constexpr auto Date = "Date";             ///< string
constexpr auto Author = "Author";         ///< string
constexpr auto Desc = "Desc";             ///< string
constexpr auto Type = "Type";             ///< string
constexpr auto MainWidget = "MainWidget"; ///< bool
constexpr auto Autorun = "Autorun";       ///< bool
constexpr auto Admin = "Admin";           ///< bool
constexpr auto Depends = "Depends";       ///< string(string list)
constexpr auto DependsPath = "DependsPath"; ///< string(path list)
constexpr auto LocalUuid = "LocalUuid"; ///< string
constexpr auto Uuid = "Uuid"; ///< string
constexpr auto ConfigPath = "ConfigPath"; ///< string(path)
} // namespace Plugin

/// Widget metadata keys.
namespace Widget {
constexpr auto Name = "Name";             ///< string
constexpr auto Desc = "Desc";             ///< string
constexpr auto ParentUuid = "ParentUuid"; ///< string
} // namespace Widget

/// Publicly accessible objects (stored in a global container).
namespace Public {
constexpr auto PManager = "WPluginManager.pluginManager"; ///< WPluginManager*
constexpr auto WManager = "WWidgetManager.widgetManager"; ///< WWidgetManager*
constexpr auto Config = "WConfigDocument.configManager";  ///< WConfigManager*
constexpr auto Path = "WPath.pathManager";                ///< WPath*
constexpr auto ServiceRegistry =
    "WServiceRegistry.serviceRegistry"; ///< WServiceRegistry*
} // namespace Public

} // namespace Consts

// Forward declarations of all framework classes

class LightWidget;
class LightSystem;
class ShellRunnable;
class WE;
class WEBase;
class WEBaseData;
class WEClass;
class WEvent;
class WMessage;
class WMetaDocument;
class WPlugin;
class WPath;
class WPluginManager;
class WPluginProxy;
class WServiceRegistry;
class WServiceProxy;
class WShellExecute;
class WVirtualPlugin;
class WWidgetManager;
using WWidget = QObject;
template <class T> class WMetaData;

} // namespace we

/**
 * @brief Obtains a pointer to a member function.
 * @param func  The member function.
 * @return A void* pointing to the function.
 *
 * @warning This function is provided **for testing purposes only**.
 *          It uses a union to convert a member function pointer to a void*,
 *          which is **not portable** and may break across compilers or
 *          calling conventions.
 */
template <typename funcType> void *getMemberAddr(funcType func) {
    union {
        void *pv;
        funcType pfn;
    } u;
    u.pfn = func;
    return u.pv;
}

#endif // WEDEF_H