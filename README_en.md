# WECore

WECore is a Qt 6-based plugin-oriented application core framework that provides foundational support for building extensible desktop applications.  
It encapsulates common functionalities such as plugin management, widget management, configuration management, path handling, service registration, crash protection, and persistent identification, allowing developers to quickly extend application capabilities through dynamic libraries or external executables (.exe / .bat).

## Key Features

### 🧩 Powerful Plugin System

- **Dual‑mode plugins**: Supports both Qt plugins (dynamic libraries) and external executables (virtual plugins).  
- **Unified interface**: All plugins only need to implement `WPluginInterface` (`init` / `recMsg` / `deinit`) to be managed by the framework.  
- **Identity and state**:  
  - **Permanent UUID**: Each plugin must provide a globally unique permanent UUID during `init()` for stable cross‑session identification, supporting crash recovery and update detection.  
  - **Plugin states**: The framework maintains a state machine: `Disabled` → `NotInstalled` → `Installed` → `Running`, and provides control operations like enable/disable/stop.  
- **Lifecycle management**: `WPluginManager` handles loading, initialization, unloading, message routing, and enforces permanent UUID validation and conflict detection.

### 🛡️ Crash Protection & Recovery

- The framework includes a built‑in crash guard singleton that writes a persistent JSON record before each call to `initPlugin()` / `initWidget()` and clears it upon success. If the process terminates unexpectedly, any leftover record identifies the component that caused the crash.  
- **Recovery workflow**: Upon startup, the framework reads leftover records and allows developers to override virtual methods to define custom recovery strategies, preventing repeated startup failures.

### 🧱 Widget Manager

- Plugins can register their own `QObject`‑derived classes as widgets with `WWidgetManager`, assigning them a stable globally unique ID (i.e., the permanent UUID).  
- Supports property access, pattern‑based event subscription/publishing, and timeout‑aware request‑reply interactions.  
- Automatically calls the initialization logic of each widget during `initWidget()` and integrates crash marking to ensure traceable exceptions.

### ⚙️ Configuration Management

- Uses `WMetaDocument` to manage JSON configuration files, supporting load/save from files or strings.  
- Provides convenient macros `PClass`, `PData`, `PPlugin` for quick access to core manager instances.

### 🌐 Service Registration & Invocation

- `WServiceRegistry` allows plugins to register named services, automatically linking them to event topics. `WServiceProxy` enables asynchronous, timeout‑aware service calls supporting both future‑based and callback‑based modes.

### 📦 Path Utilities

- `WPath` provides practical functions for obtaining executable paths, plugin paths, directory extraction, relative path resolution, etc., compatible with Windows and cross‑platform scenarios.

## Module Structure

| Class / File            | Description                                                  |
| ----------------------- | ------------------------------------------------------------ |
| `WPluginManager`        | Plugin manager responsible for plugin registration, loading, initialization, state control, message sending, permanent UUID checking, and crash recovery. |
| `WPlugin`               | Plugin instance class that encapsulates metadata, state, and lifecycle of a dynamic library or external executable. |
| `WPluginInterface`      | Pure virtual interface that all plugins must implement (`init` / `recMsg` / `deinit`). |
| `WVirtualPlugin`        | Adapter for external executables, turning process invocations into event bus interactions. |
| `WWidgetManager`        | Widget management center, responsible for widget registration, property read/write, event subscription/publishing, and crash recovery. |
| `WMetaDocument`         | JSON configuration document class derived from `WMetaData<QVariant>`, providing JSON loading and generation. |
| `WPath`                 | Path utility class supporting module path retrieval and relative/absolute path conversion. |
| `WServiceRegistry`      | Service registry that maps service names to event topics and binds them to provider lifecycles. |
| `WServiceProxy`         | Asynchronous service invocation proxy based on request/reply over the event bus. |
| `CrashGuard`            | Crash guard singleton that persists component initialization progress for automatic crash detection. |
| `WEBase` / `WEBaseData` | Global data center holding instances of managers, configuration, etc. |
| `WEClass`               | Manager factory offering accessors like `configManager()`, `pluginManager()`, etc. |

## Dependencies

- **Qt 6** (Core, Widgets modules; some features require GUI)  
- Compiler supporting **C++17** or later  
- Operating system: Windows (some path handling uses Win32 API; cross‑platform adaptation required)

## Quick Start

1. **Add WECore** to your Qt project (source code or static/dynamic library).  
2. Create a main application startup class (refer to [WidgetExplorer](https://github.com/howdy213/WidgetExplorer)), initialize `WEBase`, and load configuration and plugin lists.  
3. Implement your own plugins.  
4. Register plugin paths in the configuration file; the program will load them automatically on startup.  
5. Write widgets (inherit `QObject` and implement the `initWidget()` method), and register them via the widget manager as part of a plugin.

For detailed steps and sample code, please see the example project.

## License

WECore is licensed under the Apache License 2.0.

See the project [LICENSE](LICENSE) file for details.