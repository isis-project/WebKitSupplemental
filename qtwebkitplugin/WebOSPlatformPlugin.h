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

#ifndef WebOSPlatformPlugin_h
#define WebOSPlatformPlugin_h

#include <QObject>

class QWebSelectMethod;

namespace WebOSPlatformPlugin {

enum Extension {
    SettingsExtension = 1000
};

class ComboBoxFactory {
public:
    virtual QWebSelectMethod* createComboBox() = 0;
};

class Settings {
public:
    virtual void setComboBoxFactory(WebOSPlatformPlugin::ComboBoxFactory*) = 0;
};

}

Q_DECLARE_INTERFACE(WebOSPlatformPlugin::Settings, "com.palm.webos.platformplugin.settings/0.1");

#endif // WebOSPlatformPlugin_h
