#include "WECore/config/WConfigDef.h"
#include "WECore/config/WConfigDataDef.h"
#include <QMetaType>

namespace we::config {

QVariant defaultVariantForType(DataType type) {
    switch (type) {
    case DataType::Int:
        return 0;
    case DataType::Double:
        return 0.0;
    case DataType::String:
        return QString();
    case DataType::Bool:
        return false;
    case DataType::Array:
        return QVariantList();
    case DataType::Object:
        return QVariantMap();
    default:
        return QVariant();
    }
}

QVariant convertVariantToType(const QVariant &value, DataType targetType) {
    switch (targetType) {
    case DataType::Int:
        return value.toInt();
    case DataType::Double:
        return value.toDouble();
    case DataType::String:
        return value.toString();
    case DataType::Bool:
        return value.toBool();
    case DataType::Array:
        return value.toList();
    case DataType::Object:
        return value.toMap();
    default:
        return value;
    }
}

DataType inferDataTypeFromVariant(const QVariant &value) {
    switch (value.typeId()) {
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
        return DataType::Int;
    case QMetaType::Double:
    case QMetaType::Float:
        return DataType::Double;
    case QMetaType::QString:
        return DataType::String;
    case QMetaType::Bool:
        return DataType::Bool;
    case QMetaType::QVariantList:
        return DataType::Array;
    case QMetaType::QVariantMap:
        return DataType::Object;
    default:
        return DataType::None;
    }
}

WConfigDataBase *createDataByType(DataType type, const QString &key,
                                  const QVariant &defaultValue,
                                  const WConfigItemInfo &info,
                                  WConfigViewer *parent) {
    WConfigItemInfo finalInfo = info;
    if (finalInfo.defaultValue().isNull() && defaultValue.isValid())
        finalInfo.defaultValue(defaultValue);

    switch (type) {
    case DataType::Int: {
        auto *data = new WConfigDataInt;
        data->init(key, finalInfo, parent);
        return data;
    }
    case DataType::Double: {
        auto *data = new WConfigDataDouble;
        data->init(key, finalInfo, parent);
        return data;
    }
    case DataType::String: {
        auto *data = new WConfigDataString;
        data->init(key, finalInfo, parent);
        return data;
    }
    case DataType::Bool: {
        auto *data = new WConfigDataBool;
        data->init(key, finalInfo, parent);
        return data;
    }
    case DataType::Array: {
        auto *data = new WConfigDataArray;
        DataType elemType = info.elementType();
        if (elemType == DataType::None && defaultValue.canConvert<QVariantList>()) {
            QVariantList list = defaultValue.toList();
            if (!list.isEmpty())
                elemType = inferDataTypeFromVariant(list.first());
        }
        if (elemType == DataType::None)
            elemType = DataType::String;
        finalInfo.elementType(elemType);
        data->init(key, finalInfo, parent);
        return data;
    }
    case DataType::Select: {
        auto *data = new WConfigDataSelect;
        data->init(key, finalInfo, parent);
        if (!finalInfo.defaultValue().isValid() && !finalInfo.options().isEmpty()) {
            data->setTemporary(finalInfo.options().constFirst());
        }
        return data;
    }
    case DataType::Action: {
        auto *data = new WConfigDataAction;
        data->init(key, finalInfo, parent);
        return data;
    }
    default:
    return nullptr;
    }
}

WConfigDataBase *createDataFromVariant(const QString &key, const QVariant &value, WConfigViewer *parent)
{
    if (!value.isValid()) return nullptr;

    WConfigDataBase* newData = nullptr;

    switch (value.typeId()) {
    case QMetaType::QString: {
        auto* data = new WConfigDataString;
        WConfigItemInfo info;
        info.displayName(key).defaultValue(value.toString());
        data->init(key, info, parent);
        newData = data;
        break;
    }
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::LongLong:
    case QMetaType::ULongLong: {
        auto* data = new WConfigDataInt;
        WConfigItemInfo info;
        info.displayName(key).defaultValue(value.toInt());
        data->init(key, info, parent);
        newData = data;
        break;
    }
    case QMetaType::Double:
    case QMetaType::Float: {
        auto* data = new WConfigDataDouble;
        WConfigItemInfo info;
        info.displayName(key).defaultValue(value.toDouble());
        data->init(key, info, parent);
        newData = data;
        break;
    }
    case QMetaType::Bool: {
        auto* data = new WConfigDataBool;
        WConfigItemInfo info;
        info.displayName(key).defaultValue(value.toBool());
        data->init(key, info, parent);
        newData = data;
        break;
    }
    case QMetaType::QVariantMap: {
        auto* obj = new WConfigDataObject;
        WConfigItemInfo info;
        info.displayName(key);
        obj->init(key, info, parent);
        obj->fromVariant(value);
        newData = obj;
        break;
    }
    case QMetaType::QVariantList: {
        auto* arr = new WConfigDataArray;
        QVariantList list = value.toList();
        DataType elemType = DataType::String;
        if (!list.isEmpty()) {
            elemType = inferDataTypeFromVariant(list.first());
        }
        arr->init(key, list, elemType, Properties(), parent);
        newData = arr;
        break;
    }
    default:
        qWarning() << "createDataFromVariant: Unsupported type for key" << key
                   << "typeId:" << value.typeId();
        return nullptr;
    }
    return newData;
}

} // namespace we::config