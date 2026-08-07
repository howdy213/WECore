#include "WECore/config/WConfigTemplate.h"
#include "WECore/config/WConfigDataDef.h"
#include <QDebug>
#include <QStringList>

namespace we::config {

static WConfigDataBase *createDataCopy(const WConfigDataBase *source,
                                       WConfigViewer *parent) {
    if (!source)
        return nullptr;

    DataType type = source->type();
    WConfigDataBase *copy = createDataByType(
        type, source->key(), source->getTemporary(), source->info(), parent);
    if (copy) {
        copy->setIsFromTemplate(true);
        return copy;
    }

    if (type == DataType::Object) {
        auto *srcObj = static_cast<const WConfigDataObject *>(source);
        auto *dstObj = new WConfigDataObject;
        dstObj->init(srcObj->key(), srcObj->info(), parent);
        for (WConfigDataBase *child : srcObj->content()->allConfigData()) {
            WConfigDataBase *childCopy = createDataCopy(child, dstObj->content());
            if (childCopy)
                dstObj->addChildData(childCopy, true);
        }
        dstObj->setEditMode(srcObj->editMode());
        dstObj->setDeletionPolicy(srcObj->deletionPolicy());
        dstObj->buildDefaultMap();
        dstObj->setIsFromTemplate(true);
        return dstObj;
    }

    return nullptr;
}

void WConfigTemplate::addInt(const QString &path, const QString &key,
                             int defaultValue, const Properties &properties,
                             WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue);
    for (Property p : properties)
        info.property(p);
    addInt(path, key, info, parent);
}

void WConfigTemplate::addDouble(const QString &path, const QString &key,
                                double defaultValue, int decimals,
                                const Properties &properties,
                                WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue).decimalPlaces(decimals);
    for (Property p : properties)
        info.property(p);
    addDouble(path, key, info, parent);
}

void WConfigTemplate::addString(const QString &path, const QString &key,
                                const QString &defaultValue,
                                const Properties &properties,
                                WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue);
    for (Property p : properties)
        info.property(p);
    addString(path, key, info, parent);
}

void WConfigTemplate::addBool(const QString &path, const QString &key,
                              bool defaultValue, const Properties &properties,
                              WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue);
    for (Property p : properties)
        info.property(p);
    addBool(path, key, info, parent);
}

void WConfigTemplate::addArray(const QString &path, const QString &key,
                               const QVariantList &defaultValue,
                               DataType elementType,
                               const Properties &properties,
                               WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue).elementType(elementType);
    for (Property p : properties)
        info.property(p);
    addArray(path, key, info, parent);
}

void WConfigTemplate::addSelect(const QString &path, const QString &key,
                                const QString &defaultValue,
                                const QStringList &options,
                                const Properties &properties,
                                WConfigViewer *parent) {
    WConfigItemInfo info;
    info.defaultValue(defaultValue);
    for (Property p : properties)
        info.property(p);
    addSelect(path, key, info, parent);
}

void WConfigTemplate::addAction(const QString &path, const QString &key,
                                ActionCallback callback,
                                const Properties &properties,
                                WConfigViewer *parent) {
    WConfigItemInfo info;
    for (Property p : properties)
        info.property(p);
    info.callback(callback);
    addAction(path, key, info, parent);
}

WConfigDataObject *WConfigTemplate::addObject(const QString &path,
                                              const QString &key,
                                              const Properties &properties,
                                              WConfigViewer *parent) {
    WConfigItemInfo info;
    for (Property p : properties)
        info.property(p);
    return addObject(path, key, info, parent);
}

void WConfigTemplate::addObjectChild(WConfigDataObject *object,
                                     WConfigDataBase *childData) {
    if (!object || !childData)
        return;

    // 递归标记整个子树为模板项
    std::function<void(WConfigDataBase *)> markSubtree =
        [&](WConfigDataBase *data) {
            data->setIsFromTemplate(true);
            if (data->type() == DataType::Object) {
                auto *obj = static_cast<WConfigDataObject *>(data);
                for (auto *child : obj->content()->allConfigData()) {
                    markSubtree(child);
                }
            }
        };
    markSubtree(childData);

    object->addChildData(childData);
}

void WConfigTemplate::addInt(const QString &path, const QString &key,
                             const WConfigItemInfo &info,
                             WConfigViewer *parent) {
    WConfigViewer *viewer = findViewer(path);
    WConfigDataInt *data = new WConfigDataInt;
    data->init(key, info, parent ? parent : viewer);
    data->setIsFromTemplate(true);
    viewer->addConfigData(data);
}

void WConfigTemplate::addDouble(const QString &path, const QString &key,
                                const WConfigItemInfo &info,
                                WConfigViewer *parent) {
    WConfigViewer *viewer = findViewer(path);
    WConfigDataDouble *data = new WConfigDataDouble;
    data->init(key, info, parent ? parent : viewer);
    data->setIsFromTemplate(true);
    viewer->addConfigData(data);
}

void WConfigTemplate::addString(const QString &path, const QString &key,
                                const WConfigItemInfo &info,
                                WConfigViewer *parent) {
    WConfigViewer *viewer = findViewer(path);
    WConfigDataString *data = new WConfigDataString;
    data->init(key, info, parent ? parent : viewer);
    data->setIsFromTemplate(true);
    viewer->addConfigData(data);
}

void WConfigTemplate::addBool(const QString &path, const QString &key,
                              const WConfigItemInfo &info,
                              WConfigViewer *parent) {
    WConfigViewer *viewer = findViewer(path);
    WConfigDataBool *data = new WConfigDataBool;
    data->init(key, info, parent ? parent : viewer);
    data->setIsFromTemplate(true);
    viewer->addConfigData(data);
}

void WConfigTemplate::addArray(const QString &path, const QString &key,
                               const WConfigItemInfo &info,
                               WConfigViewer *parent) {
    WConfigViewer *viewer = findViewer(path);
    WConfigDataArray *data = new WConfigDataArray;
    data->init(key, info, parent ? parent : viewer);
    data->setIsFromTemplate(true);
    viewer->addConfigData(data);
}

void WConfigTemplate::addSelect(const QString &path, const QString &key,
                                const WConfigItemInfo &info,
                                WConfigViewer *parent) {
    WConfigViewer *viewer = findViewer(path);
    WConfigDataSelect *data = new WConfigDataSelect;
    data->init(key, info, parent ? parent : viewer);
    data->setIsFromTemplate(true);
    viewer->addConfigData(data);
}

void WConfigTemplate::addAction(const QString &path, const QString &key,
                                const WConfigItemInfo &info,
                                WConfigViewer *parent) {
    WConfigViewer *viewer = findViewer(path);
    WConfigDataAction *data = new WConfigDataAction;
    data->init(key, info, parent ? parent : viewer);
    data->setIsFromTemplate(true);
    viewer->addConfigData(data);
}

WConfigDataObject *WConfigTemplate::addObject(const QString &path,
                                              const QString &key,
                                              const WConfigItemInfo &info,
                                              WConfigViewer *parent) {
    WConfigViewer *viewer = findViewer(path);
    WConfigDataObject *data = new WConfigDataObject;
    data->init(key, info, parent ? parent : viewer);
    data->setIsFromTemplate(true);
    viewer->addConfigData(data);
    return data;
}

void WConfigTemplate::setDeletionPolicy(const QString &path,
                                        DeletionPolicy policy) {
    WConfigViewer *viewer = findViewer(path, true); // 创建路径（如果不存在）
    if (viewer)
        viewer->setDeletionPolicy(policy);
}

void WConfigTemplate::setDirectoryPolicy(const QString &path,
                                         AcceptPolicy policy,
                                         PolicyPropagation propagation,
                                         OverwriteMode overwrite,
                                         bool createIfNotExist) {
    WConfigViewer *viewer = findViewer(path, createIfNotExist);
    if (viewer)
        viewer->setDirectoryPolicy(policy, propagation, overwrite);
}

WConfigViewer *WConfigTemplate::findViewer(const QString &path,
                                           bool createIfNotExist) {
    WConfigViewer *viewer = this;
    QStringList parts = path.split("/", Qt::SkipEmptyParts);
    for (const QString &part : std::as_const(parts)) {
        WConfigViewer *child = viewer->findChildViewer(part);
        if (!child) {
            if (!createIfNotExist)
                return nullptr;
            child = new WConfigViewer(part, viewer);
            if (!viewer->addChild(child)) {
                delete child;
                child = nullptr;
                return nullptr;
            }
        }
        viewer = child;
    }
    return viewer;
}

void WConfigTemplate::applyTo(WConfigViewer *target) const {
    applyViewerTo(this, target);
}

void WConfigTemplate::setViewerMeta(const QString &path,
                                    const QString &displayName,
                                    const QString &description) {
    WConfigViewer *viewer = findViewer(path, true);
    if (viewer) {
        viewer->setDisplayName(displayName);
        viewer->setDescription(description);
    }
}

void WConfigTemplate::applyViewerTo(const WConfigViewer *source,
                                    WConfigViewer *target) const {
    if (!source || !target)
        return;
    target->setDisplayName(source->displayName());
    target->setDescription(source->description());
    for (const WConfigDataBase *sourceData : source->allConfigData()) {
        WConfigDataBase *copy = createDataCopy(sourceData, target);
        if (copy)
            target->addConfigData(copy);
    }
    for (const WConfigViewer *child : source->children()) {
        WConfigViewer *targetChild = new WConfigViewer(child->name(), target);
        target->addChild(targetChild);
        applyViewerTo(child, targetChild);
    }
    applyPolicyTo(source, target);
}

void WConfigTemplate::applyPolicyTo(const WConfigViewer *source,
                                    WConfigViewer *target) const {
    target->setDirectoryPolicy(source->acceptPolicy(),
                               PolicyPropagation::ApplyToSelfOnly,
                               OverwriteMode::ForceOverwrite);
    for (const WConfigViewer *sourceChild : source->children()) {
        WConfigViewer *targetChild = target->findChildViewer(sourceChild->name());
        if (targetChild)
            applyPolicyTo(sourceChild, targetChild);
    }
}

} // namespace we::config