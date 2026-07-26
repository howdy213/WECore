#ifndef WCONFIGDATAARRAY_H
#define WCONFIGDATAARRAY_H

#include "WConfigDataBase.h"
#include <QVariantList>

namespace we::config {

class WE_EXPORT WConfigDataArray : public WConfigDataBase {
public:
    WConfigDataArray();

    WConfigDataBase *init(const QString &key, const WConfigItemInfo &info,
                          WConfigViewer *parent = nullptr) override;
    WConfigDataArray *init(const QString &key, const QVariantList &defaultValue,
                           DataType elementType,
                           const Properties &properties = {},
                           WConfigViewer *parent = nullptr);

    QVariant getTemporary() const override { return m_value; }
    bool setTemporary(const QVariant &value) override;
    QVariant getPersistent() const override { return m_originalList; }
    bool setPersistent(const QVariant &val, bool emitSignal = true) override;
    QVariant toVariant() const override { return m_value; }
    bool fromVariant(const QVariant &variant) override;

    DataType elementType() const { return m_info.elementType(); }
    int count() const { return m_value.count(); }
    QVariant elementAt(int index) const;
    bool addElement(const QVariant &element);
    bool setElement(int index, const QVariant &element);
    bool insertElement(int index, const QVariant &element);
    bool removeElement(int index);

private:
    QVariantList m_value;
    QVariantList m_originalList;
};

} // namespace we::config

#endif // WCONFIGDATAARRAY_H