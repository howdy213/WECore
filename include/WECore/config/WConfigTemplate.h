#ifndef WCONFIGTEMPLATE_H
#define WCONFIGTEMPLATE_H

#include "WConfigDataAction.h"
#include "WConfigDataArray.h"
#include "WConfigDataObject.h"
#include "WConfigDataSelect.h"
#include "WConfigViewer.h"

namespace we::config {

class WE_EXPORT WConfigTemplate : public WConfigViewer {
public:
    WConfigTemplate() : WConfigViewer("") {}

    void addInt(const QString &path, const QString &key, int defaultValue,
                const Properties &properties = {},
                WConfigViewer *parent = nullptr);
    void addDouble(const QString &path, const QString &key, double defaultValue,
                   int decimals = 2, const Properties &properties = {},
                   WConfigViewer *parent = nullptr);
    void addString(const QString &path, const QString &key,
                   const QString &defaultValue, const Properties &properties = {},
                   WConfigViewer *parent = nullptr);
    void addBool(const QString &path, const QString &key, bool defaultValue,
                 const Properties &properties = {},
                 WConfigViewer *parent = nullptr);
    void addArray(const QString &path, const QString &key,
                  const QVariantList &defaultValue = {},
                  DataType elementType = DataType::String,
                  const Properties &properties = {},
                  WConfigViewer *parent = nullptr);
    void addSelect(const QString &path, const QString &key,
                   const QString &defaultValue, const QStringList &options,
                   const Properties &properties = {},
                   WConfigViewer *parent = nullptr);
    void addAction(const QString &path, const QString &key,
                   ActionCallback callback, const Properties &properties = {},
                   WConfigViewer *parent = nullptr);
    WConfigDataObject *addObject(const QString &path, const QString &key,
                                 const Properties &properties = {},
                                 WConfigViewer *parent = nullptr);
    void addObjectChild(WConfigDataObject *object, WConfigDataBase *childData);

    void addInt(const QString &path, const QString &key,
                const WConfigItemInfo &info, WConfigViewer *parent = nullptr);
    void addDouble(const QString &path, const QString &key,
                   const WConfigItemInfo &info, WConfigViewer *parent = nullptr);
    void addString(const QString &path, const QString &key,
                   const WConfigItemInfo &info, WConfigViewer *parent = nullptr);
    void addBool(const QString &path, const QString &key,
                 const WConfigItemInfo &info, WConfigViewer *parent = nullptr);
    void addArray(const QString &path, const QString &key,
                  const WConfigItemInfo &info, WConfigViewer *parent = nullptr);
    void addSelect(const QString &path, const QString &key,
                   const WConfigItemInfo &info, WConfigViewer *parent = nullptr);
    void addAction(const QString &path, const QString &key,
                   const WConfigItemInfo &info, WConfigViewer *parent = nullptr);
    WConfigDataObject *addObject(const QString &path, const QString &key,
                                 const WConfigItemInfo &info,
                                 WConfigViewer *parent = nullptr);

    void setDeletionPolicy(const QString &path, DeletionPolicy policy);
    void setDirectoryPolicy(
        const QString &path, AcceptPolicy policy,
        PolicyPropagation propagation = PolicyPropagation::ApplyToSelfOnly,
        OverwriteMode overwrite = OverwriteMode::KeepNonDefault,
        bool createIfNotExist = true);
    void applyTo(WConfigViewer *target) const;
    void setViewerMeta(const QString &path, const QString &displayName,
                       const QString &description);

private:
    WConfigViewer *findViewer(const QString &path, bool createIfNotExist = true);
    void applyViewerTo(const WConfigViewer *source, WConfigViewer *target) const;
    void applyPolicyTo(const WConfigViewer *source, WConfigViewer *target) const;
};

} // namespace we::config

#endif // WCONFIGTEMPLATE_H