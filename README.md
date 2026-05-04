# WECore

WECore 是一个基于 Qt 6 的插件化应用程序核心框架，为构建可扩展的桌面应用提供基础支撑。  
它封装了插件管理、Widget 管理、配置管理、路径处理、服务注册、崩溃保护以及持久化标识等通用功能，允许开发者通过动态库或外部可执行文件（.exe / .bat）快速扩展应用能力。

## 主要特性

### 🧩 强大的插件系统
- **双模式插件**：同时支持 Qt 插件（动态库）和外部可执行文件（虚拟插件）。  
- **统一接口**：所有插件只需实现 `WPluginInterface`（`init` / `recMsg` / `deinit`），即可被框架管理。  
- **身份与状态**：  
  - **永久 UUID**：每个插件在 `init()` 中必须提供一个全局唯一的永久 UUID，用于跨会话稳定识别，支持崩溃恢复与更新检测。  
  - **插件状态**：框架维护 `Disabled` → `NotInstalled` → `Installed` → `Running` 的状态机，并支持启用/禁用/停止等控制操作。  
- **生命周期管理**：`WPluginManager` 统一处理加载、初始化、卸载、消息路由，并强制执行永久 UUID 校验与冲突检测。

### 🛡️ 崩溃保护与恢复
- 框架内置崩溃标记单例，在每次调用 `initPlugin()` / `initWidget()` 前写入持久化 JSON 记录，成功后自动清除。若进程意外终止，残留记录即标识上次崩溃的组件。  
- **恢复流程**：启动读取残留记录，通过可重写的虚方法让开发者自定义处理策略，避免反复启动失败。

### 🧱 Widget 管理器
- 插件可通过 `WWidgetManager` 注册自己的 `QObject` 派生类作为 Widget，并赋予稳定的全局唯一 ID（即永久 UUID）。  
- 支持属性存取、模式匹配的事件订阅/发布，以及基于请求‑应答的超时交互。  
- 在 `initWidget()` 阶段自动调用每个 Widget 的初始化逻辑，并集成崩溃标记，确保异常可追溯。

### ⚙️ 配置管理
- 使用 `WMetaDocument`管理 JSON 配置文件，支持从文件或字符串加载/保存。  
- 提供方便的宏 `PClass`、`PData`、`PPlugin` 快速访问核心管理器实例。

### 🌐 服务注册与调用
- `WServiceRegistry` 允许插件注册命名服务，自动关联请求主题；通过 `WServiceProxy` 可进行异步、带超时的服务调用，支持 future 或回调两种模式。

### 📦 路径工具
- `WPath` 提供可执行文件路径、插件路径、目录提取、相对路径解析等实用功能，兼容 Windows 与跨平台场景。

## 模块结构

| 类 / 文件               | 功能描述                                                     |
| ----------------------- | ------------------------------------------------------------ |
| `WPluginManager`        | 插件管理器，负责插件的注册、加载、初始化、状态控制、消息发送以及永久 UUID 检查与崩溃恢复。 |
| `WPlugin`               | 插件实例类，封装动态库或外部程序的元数据、状态与生命周期。   |
| `WPluginInterface`      | 所有插件必须实现的纯虚接口（`init` / `recMsg` / `deinit`）。 |
| `WVirtualPlugin`        | 外部可执行文件的适配器，将程序调用转为事件总线交互。         |
| `WWidgetManager`        | Widget 管理中心，负责 Widget 注册、属性读写、事件订阅/发布、崩溃恢复。 |
| `WMetaDocument`         | JSON 配置文档类，继承自 `WMetaData<QVariant>`，提供 JSON 的加载与生成。 |
| `WPath`                 | 路径工具类，支持模块路径获取、相对/绝对路径转换。            |
| `WServiceRegistry`      | 服务注册表，将服务名映射到事件主题，并与提供者生命周期绑定。 |
| `WServiceProxy`         | 异步服务调用代理，基于事件总线实现请求/应答。                |                                                          |
| `WEBase` / `WEBaseData` | 全局数据中心，持有各管理器实例、配置等。                     |
| `WEClass`               | 管理器工厂，对外提供 `configManager()`、`pluginManager()` 等访问接口。 |

## 依赖

- **Qt 6**
- 操作系统：Windows（部分路径处理使用了 Win32 API，跨平台需适配）

## 快速开始

1. **引入 WECore** 到你的 Qt 项目中（源码或静态/动态库）。  
2. 创建主程序（参考 [WidgetExplorer](https://github.com/howdy213/WidgetExplorer)），初始化 `WEBase` 并加载配置与插件列表。  
3. 实现自己的插件。  
4. 在配置文件中注册插件路径，启动程序即可自动加载。  
5. 编写 Widget（继承 `QObject` 并实现 `initWidget()` 方法），通过 Widget 管理器注册为插件的一部分。

详细步骤与示例代码请参阅示例项目。

## 许可证

WECore 使用 Apache License 2.0 开源许可证

详细信息请参阅项目[LICENSE](LICENSE) 文件