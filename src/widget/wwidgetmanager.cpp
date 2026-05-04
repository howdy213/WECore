/**
 * @file wwidgetmanager.cpp
 * @brief Implementation of WWidgetManager.
 *
 * @author howdy213
 * @date 2026-05-01
 * @version 2.0.0
 *
 * Copyright 2025-2026 howdy213
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * ...
 */
#include "WECore/widget/wwidgetmanager.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QPromise>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QThread>
#include <QTimer>

#include "WECore/plugin/wplugin.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/we/webase.h"

using namespace we::Consts;

namespace we {

// Private helper structures
/**
 * @brief Represents a single event bus subscription.
 */
struct Subscription {
    QString pattern;            ///< Original pattern string (e.g., "user.*").
    QRegularExpression regex;   ///< Compiled regex for matching.
    QPointer<QObject> receiver; ///< Weak pointer to the subscriber.
    SubscribeFunc callback;     ///< The callback to invoke.

    /// Returns @c true if the subscriber still exists.
    bool valid() const { return !receiver.isNull(); }
};

/**
 * @brief Private data for WWidgetManager.
 *
 * Stores widget metadata, the subscription list, and a back‑pointer
 * to the owning WEBase instance.
 */
class WWidgetManagerPrivate {
public:
    /// Metadata associated with a registered widget.
    struct WidgetMeta {
        QUuid id;                      ///< Unique widget ID.
        QMap<QString, QVariant> attrs; ///< Arbitrary key‑value attributes.
        WPluginInterface *pluginParent = nullptr; ///< Plugin that owns the widget.
    };

    WEBase *we = nullptr; ///< Back‑pointer to the application base.
    QHash<QObject *, WidgetMeta>
        widgets; ///< Map from widget object to its metadata.
    QList<Subscription> subscriptions; ///< Active event subscriptions.
};

// Helper: compilePattern
/// Compiles a glob‑style pattern into an anchored regular expression.
QRegularExpression WWidgetManager::compilePattern(const QString &pattern) {
    if (!pattern.contains('*') && !pattern.contains('?')) {
        return QRegularExpression('^' + QRegularExpression::escape(pattern) + '$');
    }

    QString escaped = QRegularExpression::escape(pattern);
    escaped.replace(QLatin1String("\\*\\*"), QLatin1String(".*")); // ** → .*
    escaped.replace(QLatin1String("\\*"), QLatin1String("[^.]*")); // *  → [^.]*
    return QRegularExpression('^' + escaped + '$');
}

// Construction / Destruction
WWidgetManager::WWidgetManager(WEBase *base)
    : QObject(nullptr), d_ptr(new WWidgetManagerPrivate) {
    Q_D(WWidgetManager);
    d->we = base;
}

WWidgetManager::~WWidgetManager() = default;

// Widget registration & queries
bool WWidgetManager::addWidget(QUuid id, QObject *widget,
                               WPluginInterface *plugin) {
    Q_D(WWidgetManager);
    if (!widget || d->widgets.contains(widget))
        return false;

    // Ensure UUID uniqueness
    for (auto it = d->widgets.constBegin(); it != d->widgets.constEnd(); ++it) {
        if (it->id == id)
            return false;
    }

    WWidgetManagerPrivate::WidgetMeta meta;
    meta.id = id;
    meta.pluginParent = plugin;
    meta.attrs[Widget::Name] =
        QStringLiteral("Widget-") +
                               QString::number(QRandomGenerator::system()->generate64());
    d->widgets[widget] = meta;
    return true;
}

QObject *WWidgetManager::getWidget(QUuid id) {
    Q_D(WWidgetManager);
    for (auto it = d->widgets.constBegin(); it != d->widgets.constEnd(); ++it) {
        if (it->id == id)
            return it.key();
    }
    return nullptr;
}

QVector<QObject *> WWidgetManager::getWidgets() {
    Q_D(WWidgetManager);
    return {d->widgets.keyBegin(), d->widgets.keyEnd()};
}

QVector<QObject *> WWidgetManager::getWidget(const QString &key,
                                             const QVariant &value) {
    Q_D(WWidgetManager);
    QVector<QObject *> result;
    for (auto it = d->widgets.constBegin(); it != d->widgets.constEnd(); ++it) {
        if (it->attrs.value(key) == value)
            result.append(it.key());
    }
    return result;
}

QUuid WWidgetManager::getUuid(QObject *widget) {
    Q_D(WWidgetManager);
    auto it = d->widgets.find(widget);
    return (it != d->widgets.end()) ? it->id : QUuid();
}

QVariant WWidgetManager::getAttr(QObject *widget, const QString &key) {
    Q_D(WWidgetManager);
    auto it = d->widgets.find(widget);
    if (it != d->widgets.end() && it->attrs.contains(key))
        return it->attrs[key];
    return QVariant();
}

bool WWidgetManager::setAttr(QObject *widget, const QString &key,
                             const QVariant &value) {
    Q_D(WWidgetManager);
    auto it = d->widgets.find(widget);
    if (it == d->widgets.end())
        return false;

    QVariant adjusted = changeVariant(widget, key, value);
    it->attrs[key] = adjusted;
    return true;
}

void WWidgetManager::initWidget() {
    Q_D(WWidgetManager);
    auto pluginMgr = d->we->getWEClass()->pluginManager();
    for (auto it = d->widgets.begin(); it != d->widgets.end(); ++it) {
        QObject *w = it.key();
        QMetaObject::invokeMethod(w, "initWidget", Qt::DirectConnection);
        if (w->parent() == pluginMgr) {
            // The parent is the plugin manager, so retrieve the real name from the
            // plugin.
            QVariant name =
                pluginMgr->getPluginById(it->id)->getMetaData(Plugin::Name);
            this->setAttr(w, Widget::Name, name);
        }
    }
}

// Name deduplication
QVariant WWidgetManager::changeVariant(QObject *widget, const QString &key,
                                       const QVariant &value) {
    if (key != Widget::Name)
        return value;

    QString name = value.toString();
    // Look for other widgets with that name.
    QVector<QObject *> existing = getWidget(Widget::Name, name);
    while (!existing.isEmpty()) {
        // If only one widget has that name and it's the widget itself, keep the
        // name.
        if (existing.size() == 1 && existing.first() == widget)
            break;
        name += QLatin1Char('#');
        existing = getWidget(Widget::Name, name);
    }
    return name;
}

// Event bus: subscribe / unsubscribe
bool WWidgetManager::subscribe(const QString &pattern, QObject *context,
                               SubscribeFunc callback) {
    Q_D(WWidgetManager);
    if (!context || !callback || pattern.isEmpty())
        return false;

    Subscription sub;
    sub.pattern = pattern;
    sub.regex = compilePattern(pattern);
    sub.receiver = context;
    sub.callback = std::move(callback);

    // Auto‑cleanup when the receiver is destroyed.
    connect(context, &QObject::destroyed, this,
            [this, context]() { onReceiverDestroyed(context); });

    d->subscriptions.append(sub);
    return true;
}

void WWidgetManager::unsubscribeAll(QObject *receiver) {
    Q_D(WWidgetManager);
    d->subscriptions.erase(
        std::remove_if(d->subscriptions.begin(), d->subscriptions.end(),
                       [receiver](const Subscription &s) {
                           return s.receiver == receiver || s.receiver.isNull();
        }),
        d->subscriptions.end());
}

void WWidgetManager::unsubscribe(QObject *receiver, const QString &pattern) {
    Q_D(WWidgetManager);
    d->subscriptions.erase(
        std::remove_if(d->subscriptions.begin(), d->subscriptions.end(),
                       [receiver, &pattern](const Subscription &s) {
            return (s.receiver == receiver || s.receiver.isNull()) &&
                   s.pattern == pattern;
        }),
        d->subscriptions.end());
}

void WWidgetManager::onReceiverDestroyed(QObject *receiver) {
    Q_D(WWidgetManager);
    d->subscriptions.erase(
        std::remove_if(d->subscriptions.begin(), d->subscriptions.end(),
                       [receiver](const Subscription &s) {
                           return s.receiver == receiver || s.receiver.isNull();
        }),
        d->subscriptions.end());
}

// Event bus: publish
void WWidgetManager::publish(const WEvent &event) {
    // If called from a foreign thread, move the call to the manager's thread.
    if (QThread::currentThread() != this->thread()) {
        QMetaObject::invokeMethod(
            this, [this, event]() { publish(event); }, Qt::QueuedConnection);
        return;
    }

    Q_D(WWidgetManager);
    for (const auto &sub : std::as_const(d->subscriptions)) {
        if (!sub.valid())
            continue;
        if (sub.regex.match(event.topic).hasMatch()) {
            invokeCallback(sub.callback, sub.receiver, event);
        }
    }
    emit eventDispatched(event);
}

void WWidgetManager::publishSync(const WEvent &event) {
    Q_D(WWidgetManager);
    for (const auto &sub : std::as_const(d->subscriptions)) {
        if (!sub.valid())
            continue;
        if (sub.regex.match(event.topic).hasMatch()) {
            sub.callback(event);
        }
    }
}

void WWidgetManager::invokeCallback(const SubscribeFunc &cb, QObject *receiver,
                                    const WEvent &event) {
    // Invoke the callback in the receiver's thread context.
    QMetaObject::invokeMethod(
        receiver, [cb, event]() { cb(event); }, Qt::AutoConnection);
}

// Request / Response with timeout
QFuture<QVariant> WWidgetManager::request(const QString &pattern,
                                          const QVariant &data, int timeoutMs) {
    auto promise = std::make_shared<QPromise<QVariant>>();
    promise->start();
    QFuture<QVariant> future = promise->future();

    const QString correlationId =
        QUuid::createUuid().toString(QUuid::WithoutBraces);
    const QString replyPattern = QStringLiteral("_reply_") + correlationId;

    WMessage msg;
    msg.map[Data::Data] = data;
    WEvent reqEvent(pattern, msg, QStringLiteral("requestor"));
    reqEvent.correlationId = correlationId;

    auto isDone = std::make_shared<bool>(false);

    // Temporary subscription to receive the reply.
    SubscribeFunc callback = [this, promise, isDone, replyPattern,
                              correlationId](const WEvent &reply) {
        if (*isDone)
            return;
        *isDone = true;
        promise->addResult(reply.msg.map[Data::Data]);
        promise->finish();
        unsubscribe(this, replyPattern);
    };

    subscribe(replyPattern, this, std::move(callback));

    // Timeout guard.
    QTimer::singleShot(timeoutMs, this, [this, promise, isDone, replyPattern]() {
        if (!*isDone) {
            *isDone = true;
            promise->setException(
                std::make_exception_ptr(std::runtime_error("Request timed out")));
            promise->finish();
            unsubscribe(this, replyPattern);
        }
    });

    publish(reqEvent);
    return future;
}

} // namespace we