/**
 * @file wplugindata.cpp
 * @brief Implementation of the global PluginData context.
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
#include "WECore/plugin/wplugindata.h"

namespace we {
/// Holds the current global context; see PluginData.
class PluginDataPrivate {
public:
    WEBase *data = nullptr;
    WPlugin *plugin = nullptr;
    WWidget *widget = nullptr;
};

// Process-lifetime storage, allocated on first access and never freed.
PluginDataPrivate *PluginData::d = nullptr;

PluginData::PluginData() {}

PluginData::~PluginData() {}

void PluginData::setData(WEBase *data) {
    if (!d)
        d = new PluginDataPrivate;
    d->data = data;
}

WEBase *PluginData::getData() {
    if (!d)
        d = new PluginDataPrivate;
    return d->data;
}

void PluginData::setPlugin(WPlugin *plugin) {
    if (!d)
        d = new PluginDataPrivate;
    d->plugin = plugin;
}

WPlugin *PluginData::getPlugin() {
    if (!d)
        d = new PluginDataPrivate;
    return d->plugin;
}

void PluginData::setWidget(WWidget *widget) {
    if (!d)
        d = new PluginDataPrivate;
    d->widget = widget;
}

WWidget *PluginData::getWidget() {
    if (!d)
        d = new PluginDataPrivate;
    return d->widget;
}
} // namespace we