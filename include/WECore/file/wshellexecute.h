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
 * @date 2026-05-01
 * @version 2.0.0
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
 * @brief A runnable that executes a shell operation in a background thread.
 *
 * Use it together with QThreadPool for non‑blocking execution.
 */
class WE_EXPORT ShellRunnable : public QRunnable {
    Q_DISABLE_COPY(ShellRunnable)

public:
    /**
   * @brief Constructs a runnable that will call ShellExecuteW when run.
   * @param file      Path to the file or executable.
   * @param operation The verb ("open", "runas", etc.). Default is "open".
   * @param params    Command‑line parameters. Default empty.
   * @param directory Working directory (use "default" for the file’s own
   * folder).
   */
    ShellRunnable(const QString &file,
                  const QString &operation = QStringLiteral("open"),
                  const QString &params = QString(),
                  const QString &directory = QStringLiteral("default"));

    /// Destroys the runnable. Private data is automatically freed.
    ~ShellRunnable() override;

    /**
   * @brief Static helper that immediately performs the shell operation.
   * @param file      Path to the file.
   * @param operation The verb.
   * @param params    Command‑line parameters.
   * @param directory Working directory ("default" for file’s folder).
   */
    static void execute(const QString &file,
                        const QString &operation = QStringLiteral("open"),
                        const QString &params = QString(),
                        const QString &directory = QStringLiteral("default"));

protected:
    /// Called by QThreadPool; invokes execute() with the stored parameters.
    void run() override;

private:
    QScopedPointer<ShellRunnablePrivate> d_ptr;
    Q_DECLARE_PRIVATE(ShellRunnable)
};

/**
 * @brief Provides static convenience methods for shell execution.
 */
class WE_EXPORT WShellExecute {
public:
    /**
   * @brief Synchronously executes a shell operation.
   * @return Always returns @c true in the current implementation.
   */
    static bool syncExecute(const QString &file,
                            const QString &operation = QStringLiteral("open"),
                            const QString &params = QString(),
                            const QString &directory = QStringLiteral("default"));

    /**
   * @brief Asynchronously executes a shell operation using the global thread
   * pool.
   * @return Always returns @c true in the current implementation.
   */
    static bool
    asyncExecute(const QString &file,
                 const QString &operation = QStringLiteral("open"),
                 const QString &params = QString(),
                 const QString &directory = QStringLiteral("default"));
};

} // namespace we

#endif // WSHELLEXECUTE_H