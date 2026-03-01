# WECore

WECore 是一个基于 Qt 的插件化应用程序核心框架，为构建可扩展的桌面应用提供基础支持。

它封装了插件管理、Widget 管理、配置管理、路径处理等通用功能，允许开发者通过动态库（.dll）或外部可执行文件（.exe/.bat）快速扩展应用功能。

## 主要特性

- **插件系统**  
  - 支持 Qt 插件（动态库）和外部可执行文件作为插件
  - 统一的插件接口 `WPluginInterface`，提供初始化、消息接收、反初始化等生命周期管理
  - 插件元数据管理
  - 通过 `WPluginManager` 统一管理插件的加载、卸载、初始化及消息路由

- **Widget 管理**  
  - 插件可创建自己的 `WWidget` 对象，用于提供 UI 或功能模块。  
  - `WWidgetManager` 负责 Widget 的注册、查找、属性管理，并在初始化阶段自动连接信号槽。  
  - Widget 之间可通过 `sendMessage` / `receiveMessage` 进行通信。

- **配置管理**  
  - `WConfigDocument` 专门处理 JSON 格式的配置文件的加载与保存。


## 模块结构

| 类/文件                | 功能描述 |
|------------------------|----------|
| `WPluginManager`       | 插件管理器，负责插件的创建、添加、加载、卸载、消息发送等。 |
| `WPlugin`              | 插件实例类，封装动态库或外部程序的元数据与生命周期。 |
| `WPluginInterface`     | 插件必须实现的接口，包含 `init`、`recMsg`、`deinit` 等纯虚函数。 |
| `WVirtualPlugin`       | 为外部可执行文件提供的适配器，实现 `WPluginInterface`。 |
| `WWidgetManager`       | Widget 管理器，管理所有由插件创建的 `WWidget` 对象。 |
| `WWidget`              | Widget 基类，继承自 `QObject`，提供消息发送/接收、属性存取等基础功能。 |
| `WConfigDocument`      | JSON 配置文档类，继承自 `WConfig<QVariant>`，提供 JSON 加载与生成。 |

## 依赖

- Qt 6 （需包含 `Core` 模块）
- 编译器需支持 C++17

## 快速开始

见示例项目[WidgetExplorer](https://github.com/howdy213/WidgetExplorer)

## 许可证

WECore 使用 Apache License 2.0 开源许可证

详细信息请参阅项目[LICENSE](LICENSE) 文件