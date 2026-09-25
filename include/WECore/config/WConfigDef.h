/**
 * @author howdy213
 * @date 2026-09-25
 * @version 2.1.0
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
class WConfigDataCustom;
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
class WConfigEditorCustom;
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
    Action,
    Custom // user-defined data type (used with WConfigCustomTypeRegistry)
};

enum class Property {
    ReadOnly,
    RestartRequired
};

using Properties = QVector<Property>;

enum class DeletionPolicy {
    AllowAll,              // any item may be removed (template items included)
    AllowNonTemplateOnly,  // only non-template (dynamically added) items may be removed
    DisallowAll            // no item may be removed
};
QVariant defaultVariantForType(DataType type);
QVariant convertVariantToType(const QVariant& value, DataType targetType);
DataType inferDataTypeFromVariant(const QVariant& value);
/// Creates a data item of the given type; returns nullptr for an unsupported type.
/// For Array, the element type is inferred from defaultValue when not set in info.
WE_EXPORT WConfigDataBase* createDataByType(DataType type, const QString& key,
                                            const QVariant& defaultValue,
                                            const WConfigItemInfo& info,
                                            WConfigViewer* parent = nullptr);
/// Creates an item whose type is inferred from value; returns nullptr for an unsupported value.
WE_EXPORT WConfigDataBase* createDataFromVariant(const QString& key, const QVariant& value, WConfigViewer* parent = nullptr);
/// Creates a custom-type item; returns nullptr if typeName is empty or not registered.
WE_EXPORT WConfigDataBase* createCustomData(const QString& key, const QString& typeName,
                                            const QVariant& defaultValue,
                                            const WConfigItemInfo& info,
                                            WConfigViewer* parent = nullptr);
} // namespace we::config

#endif // WCONFIGDEF_H