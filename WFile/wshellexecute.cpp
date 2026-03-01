/**
 * @file wshellexecute.cpp
 * @brief ShellExecute包装类
 * @author howdy213
 * @date 2026-1-30
 * @version 1.1.0
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
#include "wshellexecute.h"
#include "wpath.h"

#include <QThreadPool>
using namespace std;

class ShellRunnablePrivate {
public:
    QString m_file;
    QString m_operation;
    QString m_params;
    QString m_directory;
};

///
/// \brief ShellRunnable::ShellRunnable
/// \param file
/// \param operation
/// \param params
/// \param directory
///
ShellRunnable::ShellRunnable(const QString &file, const QString &operation,
                             const QString &params, const QString &directory) {
    d = new ShellRunnablePrivate;
    d->m_file = file;
    d->m_directory = directory;
    d->m_operation = operation;
    d->m_params = params;
    setAutoDelete(true);
}
///
/// \brief ShellRunnable::~ShellRunnable
///
ShellRunnable::~ShellRunnable() {
    delete d;
    d = nullptr;
}
///
/// \brief ShellRunnable::run
///
void ShellRunnable::run() {
    execute(d->m_file, d->m_operation, d->m_params, d->m_directory);
}

void ShellRunnable::execute(const QString &file, const QString &operation,
                            const QString &params, const QString &directory) {
    const QString qstrDirectory =
        (directory == "default") ? WPath().splitPath(file) : directory;
    const wstring &strFile = file.toStdWString();
    const wstring &strOperation = operation.toStdWString();
    const wstring &strParams = params.toStdWString();
    const wstring &strDirectory = qstrDirectory.toStdWString();
    HINSTANCE inst =
        ShellExecuteW(0, strOperation.c_str(), strFile.c_str(), strParams.c_str(),
                                   strDirectory.c_str(), SW_NORMAL);
    Sleep(10);
    Q_UNUSED(inst);
}

bool WShellExecute::syncExecute(const QString &file, const QString &operation,
                                const QString &params,
                                const QString &directory) {
    ShellRunnable::execute(file, operation, params, directory);
    return true;
}

bool WShellExecute::asyncExecute(const QString &file, const QString &operation,
                                 const QString &params,
                                 const QString &directory) {
    ShellRunnable *runnable =
        new ShellRunnable(file, operation, params, directory);
    QThreadPool::globalInstance()->start(runnable);
    return true;
}
