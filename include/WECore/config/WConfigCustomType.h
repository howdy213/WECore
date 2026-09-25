/**
 * @author howdy213
 * @date 2026-09-25
 * @version 2.1.0
 *
 * Copyright 2025-2026 howdy213
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef WCONFIGCUSTOMTYPE_H
#define WCONFIGCUSTOMTYPE_H

#include "WECore/def/wedef.h"
#include <QString>
#include <QStringList>
#include <QVariant>
#include <functional>

class QWidget;

namespace we::config {

// Interface that custom editor widgets must implement (pure abstract, non-QObject).
// The expected editor widget inherits QWidget, returns this interface through the "out"
// parameter of editorFactory (this interface is not a QObject, so it cannot be cast with
// qobject_cast; callers may use dynamic_cast instead), and provides a parameterless signal
// named valueEdited that is emitted when the user edits the value.
class WE_EXPORT WCustomEditorInterface {
public:
    virtual ~WCustomEditorInterface() = default;
    virtual QVariant editValue() const = 0;          // Current UI value
    virtual void setEditValue(const QVariant &v) = 0; // Push a value in externally (does not emit valueEdited)
};

// Custom type descriptor. serialize() must return a QVariant that JSON/INI/QSettings can
// store, i.e. only: QString / int / qint64 / double / bool / QVariantMap / QVariantList (nested allowed).
// This is the persistence contract: toVariant() = serialize(ui value); fromVariant(s) = value = deserialize(s).
struct WE_EXPORT WConfigCustomType {
    QString typeName;
    std::function<QVariant(const QVariant &)> serialize;    // UI value -> stored value
    std::function<QVariant(const QVariant &)> deserialize;  // stored value -> UI value
    QVariant defaultValue;                                  // default UI value
    std::function<bool(const QVariant &, const QVariant &)> equal = nullptr; // defaults to QVariant==
    std::function<QString(const QVariant &)> displayString = nullptr;        // optional
    bool requireRestart = false;
    // Creates the editor widget and returns its WCustomEditorInterface* through "out" (may be the same object).
    std::function<QWidget *(QWidget *parent, WCustomEditorInterface **out)> editorFactory;
};

class WE_EXPORT WConfigCustomTypeRegistry {
public:
    static WConfigCustomTypeRegistry &instance();

    bool registerType(const WConfigCustomType &spec); // returns false if typeName already exists
    bool unregisterType(const QString &typeName);
    const WConfigCustomType *type(const QString &typeName) const;
    bool hasType(const QString &typeName) const;
    QStringList typeNames() const;

private:
    WConfigCustomTypeRegistry();
    ~WConfigCustomTypeRegistry();
    void registerBuiltins();
    Q_DISABLE_COPY(WConfigCustomTypeRegistry)
    class Impl;
    static bool buildinRegistered;
    Impl *m_impl;
};

} // namespace we::config

#endif // WCONFIGCUSTOMTYPE_H