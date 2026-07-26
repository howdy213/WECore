#ifndef WCONFIGDEF_H
#define WCONFIGDEF_H

#include "WECore/def/wedef.h"
#include <functional>
#include <QVector>

namespace we::config {

// 前向声明
class WConfig;
class WConfigDataArray;
class WConfigDataAction;
class WConfigDataBase;
class WConfigDataDouble;
class WConfigDataInt;
class WConfigDataString;
class WConfigDataBool;
class WConfigDataSelect;
class WConfigDataObject;
class WConfigViewer;
class WConfigTemplate;
class WConfigDocument;
class WConfigItemInfo;
class WConfigItemWidget;
class WConfigEditorArray;
class WConfigEditorBase;
class WConfigEditorInt;
class WConfigEditorDouble;
class WConfigEditorString;
class WConfigEditorBool;
class WConfigEditorSelect;
class WConfigEditorObject;
class WConfigEditorAction;
class NoEditColumnDelegate;
class WConfigItemRef;
class WConfigDirRef;
using ActionCallback = std::function<void()>;

enum class DataType {
    None,
    Int,
    Double,
    String,
    Bool,
    Array,
    Object,
    Select,
    Action
};

enum class Property {
    ReadOnly,
    RestartRequired
};

using Properties = QVector<Property>;

enum class DeletionPolicy {
    AllowAll,              // 允许删除任何项（包括模板项）
    AllowNonTemplateOnly,  // 只允许删除非模板项（动态添加的）
    DisallowAll            // 禁止删除任何项
};
QVariant defaultVariantForType(DataType type);
QVariant convertVariantToType(const QVariant& value, DataType targetType);
DataType inferDataTypeFromVariant(const QVariant& value);
WConfigDataBase* createDataByType(DataType type, const QString& key,
                                  const QVariant& defaultValue,
                                  const WConfigItemInfo& info,
                                  WConfigViewer* parent = nullptr);
WConfigDataBase* createDataFromVariant(const QString& key, const QVariant& value, WConfigViewer* parent = nullptr);
} // namespace we::config

#endif // WCONFIGDEF_H