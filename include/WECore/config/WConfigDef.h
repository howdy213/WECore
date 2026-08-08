/**
 * @author howdy213
 * @date 2026-08-08
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