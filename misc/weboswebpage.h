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

#ifndef weboswebpage_h
#define weboswebpage_h

#include "qwebkitglobal.h"
#include <qwebpage.h>

class WebOSWebPageCreator;
class WebOSWebPageNavigator;

class WebOSWebPage : public QWebPage {
    Q_OBJECT
public:
    WebOSWebPage(QObject* parent = 0);

    virtual ~WebOSWebPage();
    virtual bool supportsExtension(QWebPage::Extension) const;
    virtual bool extension(Extension, const ExtensionOption*, ExtensionReturn*);

    virtual const ErrorPageExtensionOption* getErrorInfo() const { return m_errorInfo; }
    void setPageCreator(WebOSWebPageCreator* creator) { m_creator = creator; }
    void setPageNavigator(WebOSWebPageNavigator* navigator) { m_navigator = navigator; }

protected:
    virtual QString userAgentForUrl(const QUrl&) const;
    virtual QWebPage* createWindow(WebWindowType);
    virtual bool acceptNavigationRequest(QWebFrame*, const QNetworkRequest&, NavigationType);
    virtual void javaScriptConsoleMessage(const QString&, int, const QString&);

private:
    const QWebPage::ErrorPageExtensionOption* m_errorInfo;
    WebOSWebPageCreator* m_creator;
    WebOSWebPageNavigator* m_navigator;
};

class WebOSWebPageCreator {
public:
     virtual WebOSWebPage* createWebOSWebPage(QWebPage::WebWindowType) = 0;
protected:
    WebOSWebPageCreator() {;}
    ~WebOSWebPageCreator() {;}
private:
    WebOSWebPageCreator(const WebOSWebPageCreator&);
    WebOSWebPageCreator& operator=(const WebOSWebPageCreator&);
};

class WebOSWebPageNavigator {
public:
     virtual bool interceptLink(const QUrl&) = 0;
protected:
    WebOSWebPageNavigator() {;}
    ~WebOSWebPageNavigator() {;}
private:
    WebOSWebPageNavigator(const WebOSWebPageNavigator&);
    WebOSWebPageNavigator& operator=(const WebOSWebPageNavigator&);
};


#endif

