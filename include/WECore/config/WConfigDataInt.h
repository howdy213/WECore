#ifndef WCONFIGDATAINT_H
#define WCONFIGDATAINT_H

#include "WConfigDataBase.h"

namespace we::config {

class WE_EXPORT WConfigDataInt : public WConfigDataBase {
public:
    WConfigDataInt();

    WConfigDataBase *init(const QString &key, const WConfigItemInfo &info,
                          WConfigViewer *parent = nullptr) override;
    WConfigDataInt *init(const QString &key, int defaultValue,
                         const Properties &properties = {},
                         WConfigViewer *parent = nullptr);

    QVariant getTemporary() const override { return m_value; }
    bool setTemporary(int value);
    bool setTemporary(const QVariant &value) override;
    QVariant getPersistent() const override { return m_original; }
    bool setPersistent(const QVariant &val, bool emitSignal = true) override;
    QVariant toVariant() const override;
    bool fromVariant(const QVariant &variant) override;

private:
    int m_value = 0;
    int m_original = 0;
};

} // namespace we::config

#endif // WCONFIGDATAINT_H