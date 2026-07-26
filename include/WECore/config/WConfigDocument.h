#ifndef WCONFIGDOCUMENT_H
#define WCONFIGDOCUMENT_H

#include "WConfigTemplate.h"
#include "WConfigViewer.h"
#include <QObject>
#include <QSettings>
#include <QString>

namespace we::config {

class WE_EXPORT WConfigDocument : public QObject {
    Q_OBJECT
public:
    enum Format { Json, Ini, Unknown };

    explicit WConfigDocument(QObject *parent = nullptr);
    ~WConfigDocument() override;

    WConfigViewer *root() const { return m_root; }
    void setTemplate(WConfigTemplate *configTemplate);
    WConfigTemplate *configTemplate() const { return m_template; }

    bool load(const QString &filePath);
    bool save(const QString &filePath, QStringList &errors);

    static Format formatFromFilePath(const QString &filePath);
    QVariant toVariant() const;
    bool clearViewer(WConfigViewer *viewer, bool force = false);
    void syncToAllPersistent();

    void setAllowCreateOnLoad(bool allow) { m_allowCreateOnLoad = allow; }
    bool loadFromSettings(QSettings* settings);
    bool saveToSettings(QSettings* settings);
private:
    bool loadJson(const QString &filePath);
    bool loadIni(const QString &filePath);
    bool saveJson(const QString &filePath);
    bool saveIni(const QString &filePath);

    void loadFromVariant(WConfigViewer *viewer, const QVariant &variant);
    QVariant saveToVariant(WConfigViewer *viewer) const;

private:
    bool m_allowCreateOnLoad = false;   // 默认为 false
    WConfigViewer *m_root;
    WConfigTemplate *m_template;
};

} // namespace we::config

#endif // WCONFIGDOCUMENT_H