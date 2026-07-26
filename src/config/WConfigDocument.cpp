#include "WECore/config/WConfigDocument.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QVariantList>
#include <QVariantMap>

namespace we::config {

WConfigDocument::WConfigDocument(QObject *parent)
    : QObject(parent), m_root(new WConfigViewer("")), m_template(nullptr) {}
WConfigDocument::~WConfigDocument() { delete m_root; }

void WConfigDocument::setTemplate(WConfigTemplate *configTemplate) {
    m_template = configTemplate;
    if (m_template) {
        clearViewer(m_root);
        m_template->applyTo(m_root);
    }
}

bool WConfigDocument::load(const QString &filePath) {
    QFileInfo info(filePath);
    if (info.suffix().compare("json", Qt::CaseInsensitive) == 0)
        return loadJson(filePath);
    else if (info.suffix().compare("ini", Qt::CaseInsensitive) == 0)
        return loadIni(filePath);
    return false;
}

bool WConfigDocument::save(const QString &filePath, QStringList &errors) {
    errors.clear();
    std::function<void(WConfigViewer *)> checkLocked =
        [&](WConfigViewer *viewer) {
        if (!viewer)
                return;
        for (auto *data : viewer->allConfigData()) {
            if (data->isEffectivelyLocked() && data->modified()) {
                errors << data->fullPath();
            }
        }
        for (auto *child : viewer->children()) {
            checkLocked(child);
        }
        };
    checkLocked(m_root);
    if (!errors.isEmpty()) {
        return false;
    }

    QFileInfo info(filePath);
    if (info.suffix().compare("json", Qt::CaseInsensitive) == 0)
        return saveJson(filePath);
    else if (info.suffix().compare("ini", Qt::CaseInsensitive) == 0)
        return saveIni(filePath);
    return false;
}

QVariant WConfigDocument::toVariant() const { return saveToVariant(m_root); }

bool WConfigDocument::loadJson(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file, using defaults.";
        return false;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qWarning() << "Invalid JSON, using defaults.";
        return false;
    }
    loadFromVariant(m_root, doc.toVariant());
    syncToAllPersistent();
    return true;
}

bool WConfigDocument::saveJson(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {  // 创建整个目录路径
            return false;
        }
    }

    QVariant variant = saveToVariant(m_root);
    QJsonDocument doc = QJsonDocument::fromVariant(variant);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

bool WConfigDocument::loadIni(const QString &filePath) {
    QSettings settings(filePath, QSettings::IniFormat);
    clearViewer(m_root);
    loadFromVariant(m_root, settings.value("config"));
    syncToAllPersistent();
    return true;
}

bool WConfigDocument::saveIni(const QString &filePath) {
    QSettings settings(filePath, QSettings::IniFormat);
    settings.setValue("config", saveToVariant(m_root));
    return true;
}
void WConfigDocument::loadFromVariant(WConfigViewer *viewer,
                                      const QVariant &variant) {
    if (!variant.canConvert<QVariantMap>())
        return;

    QVariantMap map = variant.toMap();
    WConfigViewer *tmplViewer = nullptr;
    if (m_template) {
        QString fullPath = viewer->fullPath();
        tmplViewer =
            fullPath.isEmpty() ? m_template : m_template->findChildViewer(fullPath);
    }
    bool isRoot = (viewer->parent() == nullptr);

    for (auto it = map.begin(); it != map.end(); ++it) {
        const QString &key = it.key();
        const QVariant &value = it.value();
        if (WConfigDataBase *existing = viewer->getConfigData(key)) {
            existing->fromVariant(value);
            continue;
        }
        bool isViewerInTemplate = false;
        if (tmplViewer) {
            for (WConfigViewer *child : tmplViewer->children()) {
                if (child->name() == key) {
                    isViewerInTemplate = true;
                    break;
                }
            }
        }
        if (isViewerInTemplate) {
            WConfigViewer *childViewer = viewer->findOrCreateChild(key);
            loadFromVariant(childViewer, value);
            continue;
        }
        bool canCreate = false;
        if (isRoot || m_allowCreateOnLoad) {
            canCreate = true;
        } else {
            canCreate = (viewer->effectiveAcceptPolicy() == AcceptPolicy::ACCEPT);
        }
        WConfigDataBase *tmplData =
            tmplViewer ? tmplViewer->getConfigData(key) : nullptr;
        if (!tmplData && !canCreate)
            continue;
        WConfigDataBase *newData = nullptr;
        if (isRoot && value.canConvert<QVariantMap>()) {
            bool tmplIsObject = (tmplData && tmplData->type() == DataType::Object);
            if (!tmplIsObject) {
                WConfigViewer *newChild = new WConfigViewer(key, viewer);
                if (viewer->addChild(newChild)) {
                    loadFromVariant(newChild, value);   // 递归加载子目录的内容
                } else {
                    delete newChild;
                }
                continue;
            }
        }
        if (tmplData) {
            DataType tmplType = tmplData->type();

            if (tmplType == DataType::Object) {
                auto *obj = new WConfigDataObject;
                obj->init(key, tmplData->info(), viewer);
                obj->fromVariant(value);
                newData = obj;
            } else {
                newData =
                    createDataByType(tmplType, key, value, tmplData->info(), viewer);
            }
        } else {
            DataType inferred = inferDataTypeFromVariant(value);
            if (inferred == DataType::None)
                continue;

            if (inferred == DataType::Object) {
                auto *obj = new WConfigDataObject;
                WConfigItemInfo info;
                info.displayName(key);
                obj->init(key, info, viewer);
                obj->fromVariant(value);
                newData = obj;
            } else {
                WConfigItemInfo info;
                info.displayName(key);
                newData = createDataByType(inferred, key, value, info, viewer);
            }
        }
        if (newData) {
            if (!viewer->addConfigData(newData)) {
                delete newData;
                newData = nullptr;
            }
        }
    }
}

QVariant WConfigDocument::saveToVariant(WConfigViewer *viewer) const {
    QVariantMap map;
    // 先添加数据项
    for (WConfigDataBase *data : viewer->allConfigData()) {
        map[data->key()] = data->toVariant();
    }
    // 再添加子目录
    for (WConfigViewer *child : viewer->children()) {
        if (map.contains(child->name())) {
            qWarning() << "WConfigDocument: conflict between data item and "
                          "subdirectory with same name:"
                       << child->name();
            continue;
        }
        map[child->name()] = saveToVariant(child);
    }
    return map;
}

bool WConfigDocument::clearViewer(WConfigViewer *viewer, bool force) {
    if (!force)
        if (viewer->isEffectivelyLocked())
            return false;
    for (WConfigViewer *child : viewer->children()) {
        clearViewer(child, force);
        delete child;
    }
    viewer->mutableChildren().clear();
    for (WConfigDataBase *data : viewer->allConfigData()) {
        delete data;
    }
    viewer->mutableConfigData().clear();
    return true;
}

void WConfigDocument::syncToAllPersistent() {
    std::function<void(WConfigViewer *)> traverse = [&](WConfigViewer *viewer) {
        if (!viewer)
            return;
        for (WConfigDataBase *data : viewer->allConfigData()) {
            data->syncPersistentRecursive(true);
        }
        for (WConfigViewer *child : viewer->children()) {
            traverse(child);
        }
    };
    traverse(m_root);
}
static void insertNested(QVariantMap& root, const QStringList& path, const QVariant& value) {
    if (path.isEmpty()) return;
    if (path.size() == 1) {
        root[path.first()] = value;
    } else {
        QString first = path.first();
        QVariantMap subMap = root[first].toMap();  // 获取当前子 map 副本
        insertNested(subMap, path.mid(1), value);   // 递归插入
        root[first] = subMap;                       // 写回
    }
}

// 将 QSettings 中的所有扁平键转换为嵌套 QVariantMap
static QVariantMap settingsToNestedMap(QSettings* settings) {
    QVariantMap result;
    const QStringList keys = settings->allKeys();
    for (const QString& key : keys) {
        QStringList path = key.split('/', Qt::SkipEmptyParts);
        QVariant value = settings->value(key);
        insertNested(result, path, value);
    }
    return result;
}

// 递归地将嵌套 QVariantMap 展平并写入 QSettings
static void nestedMapToSettings(const QVariantMap& map, const QString& prefix, QSettings* settings) {
    for (auto it = map.begin(); it != map.end(); ++it) {
        QString key = prefix.isEmpty() ? it.key() : prefix + "/" + it.key();
        if (it.value().typeId() == QMetaType::QVariantMap) {
            nestedMapToSettings(it.value().toMap(), key, settings);
        } else {
            settings->setValue(key, it.value());
        }
    }
}

bool WConfigDocument::loadFromSettings(QSettings* settings) {
    if (!settings) return false;
    QVariantMap nested = settingsToNestedMap(settings);
    loadFromVariant(m_root, nested);
    syncToAllPersistent();
    return true;
}

bool WConfigDocument::saveToSettings(QSettings* settings) {
    if (!settings) return false;
    QVariant nested = saveToVariant(m_root);
    nestedMapToSettings(nested.toMap(), QString(), settings);
    settings->sync(); // 立即写入
    return true;
}

} // namespace we::config