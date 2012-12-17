/* @@@LICENSE
*
*      Copyright (c) 2012 Hewlett-Packard Development Company, L.P.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
LICENSE@@@ */

#include "WebKitPlatformPlugin.h"

#include <QtPlugin>

static WebOSPlatformPlugin::ComboBoxFactory* comboboxFactory = 0;

void Settings::setComboBoxFactory(WebOSPlatformPlugin::ComboBoxFactory* factory)
{
    comboboxFactory = factory;
}

bool WebKitPlatformPlugin::supportsExtension(Extension extension) const
{
    if (extension == WebOSPlatformPlugin::SettingsExtension)
        return true;

    switch (extension) {
    case MultipleSelections:
        return comboboxFactory;
    case TouchInteraction:
        return true;
    default:
        return false;
    }
}

QObject* WebKitPlatformPlugin::createExtension(Extension extension) const
{
    if (extension == WebOSPlatformPlugin::SettingsExtension)
        return new Settings;

    switch (extension) {
    case TouchInteraction:
        return new TouchModifier();
    case MultipleSelections:
        return comboboxFactory ? comboboxFactory->createComboBox() : 0;
    default:
        return 0;
    }
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(qtwebkitplugin, WebKitPlatformPlugin)
#endif
