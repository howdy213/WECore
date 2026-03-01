/**
 * @file wpath.h
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
#ifndef WSHELLEXECUTE_H
#define WSHELLEXECUTE_H
#include <QRunnable>
#include <QString>

class ShellRunnablePrivate;
class ShellRunnable : public QRunnable {
public:
    ShellRunnable(const QString &file, const QString &operation = "open",
                  const QString &params = "",
                  const QString &directory = "default");
    ~ShellRunnable();
    static void execute(const QString &file, const QString &operation = "open",
                        const QString &params = "",
                        const QString &directory = "default");

private:
    void run() override;
    ShellRunnablePrivate *d = nullptr;
};

class WShellExecute {
public:
    static bool syncExecute(const QString &file,
                            const QString &operation = "open",
                            const QString &params = "",
                            const QString &directory = "default");
    static bool asyncExecute(const QString &file,
                             const QString &operation = "open",
                             const QString &params = "",
                             const QString &directory = "default");
};
#endif // WSHELLEXECUTE_H
