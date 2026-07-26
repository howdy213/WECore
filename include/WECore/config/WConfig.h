#ifndef WCONFIG_H
#define WCONFIG_H

#include "WECore/def/wedef.h"
#include "WConfigDocument.h"
#include <QObject>
#include <QReadWriteLock>
#include <QSettings>

namespace we::config {

class WE_EXPORT WConfig : public QObject {
    Q_OBJECT
public:
    explicit WConfig(QObject *parent = nullptr);
    ~WConfig() override;

    bool initialize(const QString &configFile,
                    WConfigTemplate *configTemplate = nullptr);
    bool initialize(QSettings* settings, WConfigTemplate* configTemplate = nullptr);
    WConfigDocument *document() const { return m_document; }

    QVariant getValueDirect(const QString &path) const;
    bool setValueDirect(const QString &path, const QVariant &value,
                        bool force = false);
    QVariant getTemporaryValue(const QString &path) const;
    bool setTemporaryValue(const QString &path, const QVariant &value);
    QVariant getValue(const QString &path) const;
    bool setValue(const QString &path, const QVariant &value, bool force = false);

    WConfigItemInfo getInfo(const QString &path) const;
    bool hasProperty(const QString &path, Property prop) const;

    QReadWriteLock *lock() const { return &m_lock; }
    QSharedPointer<WConfigItemRef> createItemRef(const QString &path);
    QSharedPointer<WConfigDirRef> createDirRef(const QString &path);

    bool save();//支持存储在QSettings中
    bool saveAs(const QString &filePath);//只支持文件
    QStringList lastSaveErrors() const;
    void resetToDefaults();

    bool hasRestartRequiredChanges() const;
    void setHasRestartRequiredChanges(bool value);

    operator QReadWriteLock *() const { return &m_lock; }
signals:
    void configChanged(we::config::WConfigDataBase *data);

private:
    QSettings* m_settings = nullptr;// 只用于以传入setting的初始化的读取/保存
    WConfigDocument *m_document;
    QString m_currentFilePath;
    bool m_hasRestartRequiredChanges;
    QStringList m_lastSaveErrors;
    mutable QReadWriteLock m_lock;
};

} // namespace we::config

#endif // WCONFIG_H