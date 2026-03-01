/**
 * @file wvirtualplugin.h
 * @brief 虚拟插件类头文件
 * @author howdy213
 * @date 2026-1-31
 * @version 1.2.0
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
#ifndef WVIRTUALPLUGIN_H
#define WVIRTUALPLUGIN_H
#include "../WDef/wedef.h"
#include "wplugininterface.h"

W_INLINE namespace WE_NAMESPACE {
    class WE_EXPORT WVirtualPlugin : public WPluginInterface {
    public:
        bool connectManager(WPluginManager *receiver,
                            bool isConnect = true) const override;

    public:
        WVirtualPlugin();
        virtual ~WVirtualPlugin();
        bool init(WMetaData &msg) override;
        void recMsg(WMetaData &msg) override;
        bool deinit(WMetaData &msg) override;

    public:
        void setFile(QString filePath) { this->m_filePath = filePath; };
        QString getFilePath() { return m_filePath; }
        void setPlugin(WPlugin *plugin);

    private:
        QString m_filePath = "";
        WPlugin *m_plugin = nullptr;
        bool m_admin = false;
    };
}
#endif // WVIRTUALPLUGIN_H
