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

#ifndef WebKitPlatformPlugin_h
#define WebKitPlatformPlugin_h

#include "qwebkitplatformplugin.h"
#include "WebOSPlatformPlugin.h"

class TouchModifier : public QWebTouchModifier
{
    Q_OBJECT
public:
    virtual unsigned hitTestPaddingForTouch(const PaddingDirection) const { return 20; }
};

class Settings : public QObject , public WebOSPlatformPlugin::Settings {
    Q_OBJECT
    Q_INTERFACES(WebOSPlatformPlugin::Settings)
public:
    virtual void setComboBoxFactory(WebOSPlatformPlugin::ComboBoxFactory* factory);
};

class WebKitPlatformPlugin : public QObject, public QWebKitPlatformPlugin
{
    Q_OBJECT
    Q_INTERFACES(QWebKitPlatformPlugin)
public:
    virtual bool supportsExtension(Extension extension) const;
    virtual QObject* createExtension(Extension extension) const;
};

#endif // WebKitPlatformPlugin_h
