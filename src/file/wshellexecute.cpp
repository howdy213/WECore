/**
 * @file wshellexecute.cpp
 * @brief Implementation of ShellRunnable and WShellExecute.
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
#include "WECore/file/wshellexecute.h"

#include <QThreadPool>

#include <Windows.h>   // ShellExecuteW (via shellapi)
#include <shellapi.h>

#include "WECore/file/wpath.h"

namespace we {

/**
 * @brief Private data for ShellRunnable.
 *
 * Stores the shell operation parameters.
 */
class ShellRunnablePrivate
{
public:
    QString m_file;       ///< Path to the file / executable.
    QString m_operation;  ///< Verb (e.g., "open", "runas").
    QString m_params;     ///< Command‑line parameters.
    QString m_directory;  ///< Working directory ("default" = file's folder).
};

ShellRunnable::ShellRunnable(const QString &file, const QString &operation,
                             const QString &params, const QString &directory)
    : d_ptr(new ShellRunnablePrivate)
{
    Q_D(ShellRunnable);
    d->m_file = file;
    d->m_directory = directory;
    d->m_operation = operation;
    d->m_params = params;
    setAutoDelete(true);
}

ShellRunnable::~ShellRunnable() = default;

void ShellRunnable::run()
{
    Q_D(ShellRunnable);
    execute(d->m_file, d->m_operation, d->m_params, d->m_directory);
}

// ---------- static execute ----------
void ShellRunnable::execute(const QString &file, const QString &operation,
                            const QString &params, const QString &directory)
{
    const QString workingDir = (directory == QStringLiteral("default"))
    ? WPath().splitPath(file)
    : directory;

    const std::wstring wFile      = file.toStdWString();
    const std::wstring wOperation = operation.toStdWString();
    const std::wstring wParams    = params.toStdWString();
    const std::wstring wDir       = workingDir.toStdWString();

    [[maybe_unused]] HINSTANCE inst =
        ShellExecuteW(nullptr, wOperation.c_str(), wFile.c_str(),
                      wParams.c_str(), wDir.c_str(), SW_NORMAL);
    Sleep(10);
    Q_UNUSED(inst);
}

bool WShellExecute::syncExecute(const QString &file, const QString &operation,
                                const QString &params, const QString &directory)
{
    ShellRunnable::execute(file, operation, params, directory);
    return true;
}

bool WShellExecute::asyncExecute(const QString &file, const QString &operation,
                                 const QString &params, const QString &directory)
{
    auto *runnable = new ShellRunnable(file, operation, params, directory);
    QThreadPool::globalInstance()->start(runnable);
    return true;
}

} // namespace we