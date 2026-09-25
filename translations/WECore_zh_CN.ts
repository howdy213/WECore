<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>WConfigFileStorage</name>
    <message>
        <location filename="../src/config/WConfigStorage.cpp" line="138"/>
        <source>No config file</source>
        <translation>未配置文件路径</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigStorage.cpp" line="179"/>
        <source>Invalid config file path</source>
        <translation>配置文件路径无效</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigStorage.cpp" line="192"/>
        <location filename="../src/config/WConfigStorage.cpp" line="199"/>
        <source>Failed to write config file: %1</source>
        <translation>写入配置文件失败：%1</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigStorage.cpp" line="204"/>
        <source>Unsupported config format: %1</source>
        <translation>不支持的配置格式：%1</translation>
    </message>
</context>
<context>
    <name>WConfigSettingsStorage</name>
    <message>
        <location filename="../src/config/WConfigStorage.cpp" line="233"/>
        <source>Invalid QSettings storage</source>
        <translation>QSettings 存储无效</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigStorage.cpp" line="255"/>
        <source>Failed to write to QSettings</source>
        <translation>写入 QSettings 失败</translation>
    </message>
</context>
<context>
    <name>WMetaDocument</name>
    <message>
        <location filename="../src/metadata/wmetadocument.cpp" line="43"/>
        <source>WMetaDocument::load: Failed to open file:</source>
        <translation>WMetaDocument::load: 打开文件失败：</translation>
    </message>
    <message>
        <location filename="../src/metadata/wmetadocument.cpp" line="57"/>
        <source>WMetaDocument::load: JSON parse error at offset</source>
        <translation>WMetaDocument::load: JSON 解析错误，偏移量</translation>
    </message>
    <message>
        <location filename="../src/metadata/wmetadocument.cpp" line="83"/>
        <source>WMetaDocument::save: Failed to open file for writing:</source>
        <translation>WMetaDocument::save: 无法打开文件进行写入：</translation>
    </message>
</context>
<context>
    <name>WPlugin</name>
    <message>
        <location filename="../src/plugin/wplugin.cpp" line="119"/>
        <source>WPlugin::load: Cannot transition to loading state</source>
        <translation>WPlugin::load: 无法切换到加载状态</translation>
    </message>
    <message>
        <location filename="../src/plugin/wplugin.cpp" line="137"/>
        <source>WPlugin::load: Unsupported plugin file type</source>
        <translation>WPlugin::load: 不支持的插件文件类型</translation>
    </message>
    <message>
        <location filename="../src/plugin/wplugin.cpp" line="159"/>
        <source>WPlugin::unload: Cannot transition to unloading state</source>
        <translation>WPlugin::unload: 无法切换到卸载状态</translation>
    </message>
    <message>
        <location filename="../src/plugin/wplugin.cpp" line="234"/>
        <source>WPlugin::loadDll: Plugin does not implement WPluginInterface</source>
        <translation>WPlugin::loadDll: 插件未实现 WPluginInterface 接口</translation>
    </message>
    <message>
        <location filename="../src/plugin/wplugin.cpp" line="240"/>
        <source>WPlugin::loadDll: Failed to load %1 - %2</source>
        <translation>WPlugin::loadDll: 加载 %1 失败 - %2</translation>
    </message>
</context>
<context>
    <name>we::WPluginManager</name>
    <message>
        <location filename="../src/plugin/wpluginmanager.cpp" line="85"/>
        <source>WPluginManager::loadPlugin: Plugin is null</source>
        <translation>WPluginManager::loadPlugin: 插件为空</translation>
    </message>
    <message>
        <location filename="../src/plugin/wpluginmanager.cpp" line="91"/>
        <source>WPluginManager::loadPlugin: Plugin not found in registry: %1</source>
        <translation>WPluginManager::loadPlugin: 注册表中未找到插件：%1</translation>
    </message>
</context>
<context>
    <name>we::WPluginStateMachine</name>
    <message>
        <location filename="../src/plugin/wpluginstatemachine.cpp" line="38"/>
        <source>Unloaded</source>
        <translation>已卸载</translation>
    </message>
    <message>
        <location filename="../src/plugin/wpluginstatemachine.cpp" line="40"/>
        <source>Loading</source>
        <translation>加载中</translation>
    </message>
    <message>
        <location filename="../src/plugin/wpluginstatemachine.cpp" line="42"/>
        <source>Loaded</source>
        <translation>已加载</translation>
    </message>
    <message>
        <location filename="../src/plugin/wpluginstatemachine.cpp" line="44"/>
        <source>Unloading</source>
        <translation>卸载中</translation>
    </message>
    <message>
        <location filename="../src/plugin/wpluginstatemachine.cpp" line="46"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../src/plugin/wpluginstatemachine.cpp" line="48"/>
        <source>Disabled</source>
        <translation>已禁用</translation>
    </message>
    <message>
        <location filename="../src/plugin/wpluginstatemachine.cpp" line="50"/>
        <source>Unknown</source>
        <translation>未知</translation>
    </message>
    <message>
        <location filename="../src/plugin/wpluginstatemachine.cpp" line="112"/>
        <source>Invalid transition from %1 to %2</source>
        <translation>无效的状态转换：%1 → %2</translation>
    </message>
</context>
<context>
    <name>we::WVirtualPlugin</name>
    <message>
        <location filename="../src/plugin/wvirtualplugin.cpp" line="98"/>
        <source>WVirtualPlugin::init called without a plugin metadata object</source>
        <translation>WVirtualPlugin::init 调用时缺少插件元数据对象</translation>
    </message>
</context>
<context>
    <name>we::WWidgetManager</name>
    <message>
        <location filename="../src/widget/wwidgetmanager.cpp" line="325"/>
        <source>Request timed out</source>
        <translation>请求超时</translation>
    </message>
</context>
<context>
    <name>we::config::WConfig</name>
    <message>
        <location filename="../src/config/WConfig.cpp" line="143"/>
        <source>No storage backend is configured</source>
        <translation>未配置存储后端</translation>
    </message>
</context>
<context>
    <name>we::config::WConfigEditorArray</name>
    <message>
        <location filename="../src/config/WConfigEditorArray.cpp" line="39"/>
        <source>Index</source>
        <translation>索引</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorArray.cpp" line="39"/>
        <source>Value</source>
        <translation>值</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorArray.cpp" line="49"/>
        <source>Append Element</source>
        <translation>追加元素</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorArray.cpp" line="50"/>
        <source>Insert Before</source>
        <translation>向前插入</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorArray.cpp" line="51"/>
        <source>Insert After</source>
        <translation>向后插入</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorArray.cpp" line="52"/>
        <source>Remove Selected</source>
        <translation>删除所选</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorArray.cpp" line="141"/>
        <source>No Selection</source>
        <translation>未选择</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorArray.cpp" line="142"/>
        <source>Please select an element first.</source>
        <translation>请先选择一个元素。</translation>
    </message>
</context>
<context>
    <name>we::config::WConfigEditorObject</name>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="46"/>
        <source>Key</source>
        <translation>键</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="46"/>
        <source>Value</source>
        <translation>值</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="65"/>
        <source>Add Child</source>
        <translation>添加子项</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="66"/>
        <source>Remove Selected</source>
        <translation>删除所选</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="245"/>
        <source>Not Allowed</source>
        <translation>不允许</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="246"/>
        <source>Cannot add child items in current edit mode (requires FullControl).</source>
        <translation>当前编辑模式下无法添加子项（需要 FullControl）。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="270"/>
        <source>Add Child Item</source>
        <translation>添加子项</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="274"/>
        <source>Key:</source>
        <translation>键：</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="278"/>
        <source>Type:</source>
        <translation>类型：</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="281"/>
        <source>Value:</source>
        <translation>值：</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="307"/>
        <location filename="../src/config/WConfigEditorObject.cpp" line="311"/>
        <location filename="../src/config/WConfigEditorObject.cpp" line="344"/>
        <location filename="../src/config/WConfigEditorObject.cpp" line="351"/>
        <location filename="../src/config/WConfigEditorObject.cpp" line="362"/>
        <location filename="../src/config/WConfigEditorObject.cpp" line="380"/>
        <location filename="../src/config/WConfigEditorObject.cpp" line="391"/>
        <location filename="../src/config/WConfigEditorObject.cpp" line="409"/>
        <location filename="../src/config/WConfigEditorObject.cpp" line="440"/>
        <location filename="../src/config/WConfigEditorObject.cpp" line="447"/>
        <location filename="../src/config/WConfigEditorObject.cpp" line="480"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="307"/>
        <source>Key cannot be empty.</source>
        <translation>键不能为空。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="312"/>
        <source>Child with key &apos;%1&apos; already exists.</source>
        <translation>键为“%1”的子项已存在。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="320"/>
        <source>Depth Limit Exceeded</source>
        <translation>超出层级限制</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="321"/>
        <source>Cannot add nested object: maximum depth (%1) reached.</source>
        <translation>无法添加嵌套对象：已达到最大层级（%1）。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="344"/>
        <source>Invalid integer.</source>
        <translation>整数无效。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="351"/>
        <source>Invalid double.</source>
        <translation>浮点数无效。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="363"/>
        <source>Invalid boolean (true/false).</source>
        <translation>布尔值无效（true/false）。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="380"/>
        <source>Failed to create item.</source>
        <translation>创建项失败。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="392"/>
        <source>An item with key &apos;%1&apos; already exists.</source>
        <translation>键为“%1”的项已存在。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="409"/>
        <source>Failed to add child item.</source>
        <translation>添加子项失败。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="426"/>
        <source>Operation Not Allowed</source>
        <translation>操作不允许</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="427"/>
        <source>Current object edit mode does not allow deletion.</source>
        <translation>当前对象编辑模式不允许删除。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="432"/>
        <source>No Selection</source>
        <translation>未选择</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="433"/>
        <source>Please select an item to remove.</source>
        <translation>请选择要删除的项。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="440"/>
        <source>Invalid selected item.</source>
        <translation>所选项目无效。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="448"/>
        <source>Cannot determine parent object (data corruption).</source>
        <translation>无法确定父对象（数据损坏）。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="454"/>
        <source>Deletion Forbidden</source>
        <translation>禁止删除</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="455"/>
        <source>Deletion is not allowed for items in this object.</source>
        <translation>该对象中的项不允许删除。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="459"/>
        <source>Cannot Delete Template Item</source>
        <translation>无法删除模板项</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="460"/>
        <source>This item is defined by the template and cannot be deleted.</source>
        <translation>该项由模板定义，无法删除。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="469"/>
        <source>Confirm Deletion</source>
        <translation>确认删除</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="470"/>
        <source>Are you sure you want to delete the following child item?

Key:   %1
Value: %2</source>
        <translation>确定要删除以下子项吗？

键：  %1
值： %2</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="480"/>
        <source>Item has no parent container.</source>
        <translation>该项没有父容器。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="485"/>
        <source>Deletion Failed</source>
        <translation>删除失败</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigEditorObject.cpp" line="486"/>
        <source>Failed to remove the item from its parent container.</source>
        <translation>从父容器中移除该项失败。</translation>
    </message>
</context>
<context>
    <name>we::config::WConfigItemWidget</name>
    <message>
        <location filename="../src/config/WConfigItemWidget.cpp" line="163"/>
        <source>Reset to default value</source>
        <translation>恢复默认值</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigItemWidget.cpp" line="171"/>
        <source>Undo changes</source>
        <translation>撤销修改</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigItemWidget.cpp" line="299"/>
        <source>No Default</source>
        <translation>无默认值</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigItemWidget.cpp" line="300"/>
        <source>This item has no default value.</source>
        <translation>该项没有默认值。</translation>
    </message>
</context>
<context>
    <name>we::config::WConfigPathEditor</name>
    <message>
        <location filename="../src/config/WConfigCustomBuiltins.cpp" line="30"/>
        <source>Browse...</source>
        <translation>浏览...</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigCustomBuiltins.cpp" line="32"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigCustomBuiltins.cpp" line="63"/>
        <source>Select Directory</source>
        <translation>选择目录</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigCustomBuiltins.cpp" line="64"/>
        <source>Select File</source>
        <translation>选择文件</translation>
    </message>
</context>
<context>
    <name>we::config::WConfigWidget</name>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="46"/>
        <source>Some settings will take effect after restart.</source>
        <translation>部分设置将在重启后生效。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="47"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="48"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="49"/>
        <source>Add Item</source>
        <translation>添加项</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="50"/>
        <source>Remove Selected</source>
        <translation>删除所选</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="51"/>
        <source>Settings</source>
        <translation>设置</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="67"/>
        <source>Root</source>
        <translation>根</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="484"/>
        <source>Some settings require restart to take effect.</source>
        <translation>部分设置需要重启后生效。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="486"/>
        <source>No restart required.</source>
        <translation>无需重启。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="507"/>
        <source>Save failed due to locked items with changes:

</source>
        <translation>以下锁定项存在修改，保存失败：

</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="509"/>
        <location filename="../src/config/WConfigWidget.cpp" line="511"/>
        <source>Save Failed</source>
        <translation>保存失败</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="511"/>
        <source>Unknown save error.</source>
        <translation>未知的保存错误。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="532"/>
        <source>Add Configuration Item</source>
        <translation>添加配置项</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="536"/>
        <source>Key:</source>
        <translation>键：</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="540"/>
        <source>Type:</source>
        <translation>类型：</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="543"/>
        <source>Value:</source>
        <translation>值：</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="556"/>
        <location filename="../src/config/WConfigWidget.cpp" line="560"/>
        <location filename="../src/config/WConfigWidget.cpp" line="576"/>
        <location filename="../src/config/WConfigWidget.cpp" line="584"/>
        <location filename="../src/config/WConfigWidget.cpp" line="596"/>
        <location filename="../src/config/WConfigWidget.cpp" line="607"/>
        <location filename="../src/config/WConfigWidget.cpp" line="614"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="556"/>
        <source>Key cannot be empty.</source>
        <translation>键不能为空。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="561"/>
        <source>Item with key &apos;%1&apos; already exists.</source>
        <translation>键为“%1”的项已存在。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="576"/>
        <source>Invalid integer.</source>
        <translation>整数无效。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="584"/>
        <source>Invalid double.</source>
        <translation>浮点数无效。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="597"/>
        <source>Invalid boolean (true/false).</source>
        <translation>布尔值无效（true/false）。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="607"/>
        <source>Failed to create item.</source>
        <translation>创建项失败。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="615"/>
        <source>Failed to add item &apos;%1&apos;.</source>
        <translation>添加项“%1”失败。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="627"/>
        <source>Info</source>
        <translation>提示</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="628"/>
        <source>Please select an item first.</source>
        <translation>请先选择一个项。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="638"/>
        <source>Deletion Forbidden</source>
        <translation>禁止删除</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="639"/>
        <source>Deletion is not allowed in this directory.</source>
        <translation>该目录下不允许删除。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="645"/>
        <source>Cannot Delete Template Item</source>
        <translation>无法删除模板项</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="646"/>
        <source>This item is defined by the template and cannot be deleted.</source>
        <translation>该项由模板定义，无法删除。</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="670"/>
        <source>Confirm Delete</source>
        <translation>确认删除</translation>
    </message>
    <message>
        <location filename="../src/config/WConfigWidget.cpp" line="671"/>
        <source>Are you sure you want to delete the item:
Key: %1
Type: %2
Value: %3</source>
        <translation>确定要删除该项吗：
键： %1
类型： %2
值： %3</translation>
    </message>
</context>
</TS>
