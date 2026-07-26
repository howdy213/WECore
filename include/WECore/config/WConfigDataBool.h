#ifndef WCONFIGDATABOOL_H
#define WCONFIGDATABOOL_H

#include "WConfigDataBase.h"

namespace we::config {

class WE_EXPORT WConfigDataBool : public WConfigDataBase {
public:
    WConfigDataBool();

    WConfigDataBase *init(const QString &key, const WConfigItemInfo &info,
                          WConfigViewer *parent = nullptr) override;
    WConfigDataBool *init(const QString &key, bool defaultValue,
                          const Properties &properties = {},
                          WConfigViewer *parent = nullptr);

    QVariant getTemporary() const override { return m_value; }
    bool setTemporary(bool value);
    bool setTemporary(const QVariant &value) override;
    QVariant getPersistent() const override { return m_original; }
    bool setPersistent(const QVariant& val, bool emitSignal = true) override;
    bool setPersistent(bool val);
    QVariant toVariant() const override;
    bool fromVariant(const QVariant &variant) override;

private:
    bool m_value = false;
    bool m_original = false;
};

} // namespace we::config

#endif // WCONFIGDATABOOL_H