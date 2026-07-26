#ifndef WCONFIGDATABASE_H
#define WCONFIGDATABASE_H

#include "WConfigDef.h"
#include "WConfigItemInfo.h"
#include <QString>
#include <QVariant>
#include <functional>

namespace we::config {

class WConfigViewer;

class WE_EXPORT WConfigDataBase {
public:
    WConfigDataBase(const WConfigDataBase &) = delete;
    WConfigDataBase &operator=(const WConfigDataBase &) = delete;
    WConfigDataBase(WConfigDataBase &&) = delete;
    WConfigDataBase &operator=(WConfigDataBase &&) = delete;
    virtual ~WConfigDataBase() { notifyDestroy(); }

    virtual WConfigDataBase *init(const QString &key, const WConfigItemInfo &info,
                                  WConfigViewer *parent = nullptr) = 0;

    QString fullPath() const;
    DataType type() const { return m_type; }
    QString key() const { return m_key; }
    WConfigViewer *parent() const { return m_parent; }
    const WConfigItemInfo &info() const { return m_info; }
    void setInfo(const WConfigItemInfo &info) { m_info = info; }
    void setParent(WConfigViewer *parent) { m_parent = parent; }
    bool isFromTemplate() const { return m_isFromTemplate; }
    void setIsFromTemplate(bool from) { m_isFromTemplate = from; }

    virtual QVariant getTemporary() const = 0;
    // 若设置值与原值不同返回true
    virtual bool setTemporary(const QVariant &value) = 0;
    virtual QVariant getPersistent() const = 0;
    virtual bool setPersistent(const QVariant &val, bool emitSignal = true) = 0;
    virtual bool revertToPersistent();
    virtual bool revertToDefault();
    virtual bool syncPersistent(bool emitSignal = true);
    virtual bool modified() const;

    virtual void forEachChild(std::function<void(WConfigDataBase *)>) const {}
    virtual void syncPersistentRecursive(bool emitSignal = true) {
        syncPersistent(emitSignal);
    }
    virtual void revertToPersistentRecursive() { revertToPersistent(); }

    virtual QVariant toVariant() const = 0;
    virtual bool fromVariant(const QVariant &variant) = 0;

    bool hasProperty(Property prop) const;

public:
    using ChangeCallback = std::function<void(WConfigDataBase *)>;
    void addObserver(WConfigItemRef *observer);
    void removeObserver(WConfigItemRef *observer);
    void lock() { ++m_lockCount; }
    void unlock() {
        if (m_lockCount > 0)
            --m_lockCount;
    }
    bool isLocked() const { return m_lockCount > 0; }
    bool isEffectivelyLocked() const;

protected:
    WConfigDataBase() = default;
    void init(DataType type, const QString &key, const WConfigItemInfo &info,
              WConfigViewer *parent = nullptr);
    void notifyChange();
    void notifyDestroy();
    bool m_isFromTemplate = false;
    DataType m_type = DataType::None;
    QString m_key;
    WConfigItemInfo m_info;
    WConfigViewer *m_parent = nullptr;

    QList<WConfigItemRef *> m_observers;
    int m_lockCount = 0;
};

} // namespace we::config

#endif // WCONFIGDATABASE_H