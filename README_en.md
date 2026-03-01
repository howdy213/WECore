# WECore

WECore is a Qt-based plugin-based application core framework that provides foundational support for building extensible desktop applications.

It encapsulates common functionalities such as plugin management, Widget management, configuration management, and path handling, allowing developers to quickly extend application features through dynamic libraries (.dll) or external executable files (.exe/.bat).

## Key Features

- **Plugin System**
  - Supports both Qt plugins (dynamic libraries) and external executables as plugins
  - Unified plugin interface `WPluginInterface` providing lifecycle management including initialization, message reception, and deinitialization
  - Plugin metadata management
  - Unified management of plugin loading, unloading, initialization, and message routing via `WPluginManager`

- **Widget Management**
  - Plugins can create their own `WWidget` objects to provide UI or functional modules
  - `WWidgetManager` is responsible for widget registration, lookup, property management, and automatically connects signals and slots during initialization
  - Widgets can communicate with each other via `sendMessage` / `receiveMessage`

- **Configuration Management**
  - `WConfigDocument` specifically handles loading and saving of JSON format configuration files

## Module Structure

| Class/File         | Description                                                  |
| ------------------ | ------------------------------------------------------------ |
| `WPluginManager`   | Plugin manager responsible for plugin creation, addition, loading, unloading, message sending, etc. |
| `WPlugin`          | Plugin instance class that encapsulates metadata and lifecycle of dynamic libraries or external programs |
| `WPluginInterface` | Interface that plugins must implement, containing pure virtual functions such as `init`, `recMsg`, `deinit` |
| `WVirtualPlugin`   | Adapter for external executables, implementing `WPluginInterface` |
| `WWidgetManager`   | Widget manager that manages all `WWidget` objects created by plugins |
| `WWidget`          | Widget base class, inheriting from `QObject`, providing basic functionalities such as message sending/receiving, property access |
| `WConfigDocument`  | JSON configuration document class, inheriting from `WConfig<QVariant>`, providing JSON loading and generation |

## Dependencies

- Qt 6 (requires `Core` module)
- Compiler must support C++17

## Quick Start

See the example project [WidgetExplorer](https://github.com/howdy213/WidgetExplorer)

## License

WECore is licensed under the Apache License 2.0

For more details, please refer to the project [LICENSE](LICENSE) file.