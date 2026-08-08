English | [中文](README.md)

# WECore

WECore is a Qt 6‑based plugin‑oriented application core framework that provides foundational support for building extensible desktop applications.  
It encapsulates common functionality such as plugin management, widget management, configuration handling, path utilities, service registration, crash protection, and persistent identification, allowing developers to rapidly extend application capabilities through dynamic libraries or external executables (.exe / .bat).

## Key Features

### 🧩 Powerful Plugin System

- **Dual‑mode plugins**: supports both Qt plugins (dynamic libraries) and external executables (virtual plugins) simultaneously.  
- **Unified interface**: every plugin only needs to implement `WPluginInterface` (`init` / `recMsg` / `deinit`) to be managed by the framework.  
- **Identity and state**:  
  - **Permanent UUID**: each plugin must provide a globally unique, permanently fixed UUID in its `init()` method. This UUID enables stable identification across sessions and supports crash recovery and update detection.  
  - **Plugin state**: the framework maintains a state machine with the transitions `Disabled` → `NotInstalled` → `Installed` → `Running`, and supports control operations such as enable, disable, and stop.  
- **Lifecycle management**: `WPluginManager` uniformly handles loading, initialization, unloading, and message routing, and enforces permanent UUID validation and conflict detection.

### 🛡️ Crash Protection and Recovery

- The framework incorporates a crash‑marker singleton. Before each call to `initPlugin()` / `initWidget()`, a persistent JSON record is written; it is automatically cleared upon success. If the process terminates unexpectedly, the remaining record identifies the component that crashed last.  
- **Recovery process**: on startup, leftover records are read, and overridable virtual methods allow developers to customize the handling strategy, preventing repeated startup failures.

### 🧱 Widget Manager

- Plugins can register their own `QObject`‑derived classes as widgets through `WWidgetManager`, assigning a stable globally unique ID (i.e., the permanent UUID).  
- Supports property access, pattern‑matching event subscribe/publish, and timeout‑based request‑reply interaction.  
- During the `initWidget()` stage, each widget’s initialization logic is automatically invoked, and crash markers are integrated to ensure traceability of exceptions.

### ⚙️ Configuration Management

- Uses `WMetaDocument` to manage JSON configuration files, supporting load/save from files or strings.  
- Provides convenient macros `PClass`, `PData`, `PPlugin` for quick access to core manager instances.
- Offers the `WConfig` library for convenient configuration management, supporting typed configuration items, nested structures, JSON/INI serialization, UI integration, thread‑safe access, and template‑based structure definitions.

### 🌐 Service Registration and Invocation

- `WServiceRegistry` allows plugins to register named services, automatically associating them with request topics; `WServiceProxy` enables asynchronous, timeout‑enabled service calls, supporting both future and callback patterns.

### 📦 Path Utilities

- `WPath` provides practical functions such as executable path, plugin path, directory extraction, and relative path resolution, compatible with Windows and cross‑platform scenarios.

## Module Structure

| Class / File            | Description                                                  |
| ----------------------- | ------------------------------------------------------------ |
| `WPluginManager`        | Plugin manager, responsible for plugin registration, loading, initialization, state control, message dispatch, permanent UUID checking, and crash recovery. |
| `WPlugin`               | Plugin instance class, encapsulating metadata, state, and lifecycle of a dynamic library or external program. |
| `WPluginInterface`      | Pure virtual interface that every plugin must implement (`init` / `recMsg` / `deinit`). |
| `WVirtualPlugin`        | Adapter for external executables, converting program invocation into event‑bus interaction. |
| `WWidgetManager`        | Widget management center, responsible for widget registration, property reading/writing, event subscribe/publish, and crash recovery. |
| `WMetaDocument`         | JSON configuration document class, inheriting from `WMetaData<QVariant>`, providing JSON loading and generation. |
| `WPath`                 | Path utility class, supporting module path retrieval and relative/absolute path conversion. |
| `WServiceRegistry`      | Service registry that maps service names to event topics and binds them to the provider’s lifecycle. |
| `WServiceProxy`         | Asynchronous service invocation proxy based on the event bus, implementing request/reply. |
| `WEBase` / `WEBaseData` | Global data center, holding manager instances, configuration, etc. |
| `WEClass`               | Manager factory, providing external access interfaces such as `configManager()`, `pluginManager()`. |
| `WConfig` library       | Flexible, hierarchical Qt configuration management system.   |

## Dependencies

- **Qt 6**
- Operating system: Windows; cross‑platform adaptation required.

## Quick Start

1. **Add WECore** to your Qt project (source code or static/dynamic library).  
2. Create the main application (refer to [WidgetExplorer](https://github.com/howdy213/WidgetExplorer)), initialize `WEBase`, and load configuration and plugin list.  
3. Implement your own plugins.  
4. Register the plugin paths in the configuration file; they will be loaded automatically when the application starts.  
5. Write widgets (inherit `QObject` and implement the `initWidget()` method) and register them with the Widget Manager as part of a plugin.  
6. Implement service registration, communication, and other functionalities.

For detailed steps and example code, please refer to the sample projects.

## License

WECore is released under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.

Qt: [LGPLv3 License](licenses/LICENSE.LESSER-Qt)