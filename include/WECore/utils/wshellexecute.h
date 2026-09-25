/**
 * @file wshellexecute.h
 * @brief Wrappers around Windows ShellExecute for synchronous and asynchronous
 * execution.
 *
 * ShellRunnable implements QRunnable for background execution, and
 * WShellExecute provides convenient static methods for both synchronous
 * and asynchronous calls.
 *
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
#ifndef WSHELLEXECUTE_H
#define WSHELLEXECUTE_H

#include <QRunnable>
#include <QScopedPointer>
#include <QString>

#include "WECore/def/wedef.h"

namespace we {

class ShellRunnablePrivate;

/**
 * @brief A runnable that executes a shell operation on a thread‑pool thread.
 *
 * The parameters are captured at construction and forwarded by run();
 * the instance deletes itself once run() returns.
 */
class WE_EXPORT ShellRunnable : public QRunnable {
    Q_DISABLE_COPY(ShellRunnable)

public:
    /// @p directory "default" means "use the folder of @p file".
    ShellRunnable(const QString &file,
                  const QString &operation = QStringLiteral("open"),
                  const QString &params = QString(),
                  const QString &directory = QStringLiteral("default"));

    ~ShellRunnable() override;

    /**
   * @brief Performs the shell operation in the calling thread.
   *
   * @p params is passed to ShellExecuteW verbatim; quoting is the caller's
   * responsibility.
   */
    static void execute(const QString &file,
                        const QString &operation = QStringLiteral("open"),
                        const QString &params = QString(),
                        const QString &directory = QStringLiteral("default"));

protected:
    /// Called by QThreadPool; forwards the stored parameters to execute().
    void run() override;

private:
    QScopedPointer<ShellRunnablePrivate> d_ptr;
    Q_DECLARE_PRIVATE(ShellRunnable)
};

/**
 * @brief Static facade over the Windows shell (ShellExecuteW); fire‑and‑forget.
 */
class WE_EXPORT WShellExecute {
public:
    /// Runs the operation on the calling thread; always returns @c true (failures are unreported).
    static bool syncExecute(const QString &file,
                            const QString &operation = QStringLiteral("open"),
                            const QString &params = QString(),
                            const QString &directory = QStringLiteral("default"));

    /// Queues a ShellRunnable on the global thread pool; always returns @c true.
    static bool
    asyncExecute(const QString &file,
                 const QString &operation = QStringLiteral("open"),
                 const QString &params = QString(),
                 const QString &directory = QStringLiteral("default"));
};

} // namespace we

#endif // WSHELLEXECUTE_H