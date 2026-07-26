#ifndef WCONFIGDATASTRING_H
#define WCONFIGDATASTRING_H

#include "WConfigDataBase.h"

namespace we::config {

class WE_EXPORT WConfigDataString : public WConfigDataBase
{
public:
    WConfigDataString();

    WConfigDataBase* init(const QString& key, const WConfigItemInfo& info, WConfigViewer* parent = nullptr) override;
    WConfigDataString* init(const QString& key, const QString& defaultValue, const Properties& properties = {}, WConfigViewer* parent = nullptr);

    QVariant getTemporary() const override { return m_value; }
    bool setTemporary(const QString& value);
    bool setTemporary(const QVariant& value) override;
    QVariant getPersistent() const override { return m_original; }
    bool setPersistent(const QVariant& val, bool emitSignal = true) override;
    QVariant toVariant() const override;
    bool fromVariant(const QVariant& variant) override;

private:
    QString m_value;
    QString m_original;
};

} // namespace we::config

#endif // WCONFIGDATASTRING_H