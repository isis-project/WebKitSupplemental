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

#include "weboswebpage.h"

#include <QtCore/QSettings>
#include <QtNetwork/QNetworkRequest>


WebOSWebPage::WebOSWebPage(QObject* parent)
    : QWebPage(parent)
    , m_errorInfo(0)
    , m_creator(0)
    , m_navigator(0)
{
}

bool WebOSWebPage::supportsExtension(QWebPage::Extension extension) const
{
    if (extension == QWebPage::ErrorPageExtension)
        return true;
    return false;
}

bool WebOSWebPage::extension(Extension extension, const ExtensionOption* option, ExtensionReturn* output)
{
    Q_UNUSED(output);
    if (extension != QWebPage::ErrorPageExtension)
        return false;
    if (m_errorInfo)
        delete m_errorInfo;
    m_errorInfo = new QWebPage::ErrorPageExtensionOption(*static_cast<const QWebPage::ErrorPageExtensionOption*>(option));
    return false;
}

QString WebOSWebPage::userAgentForUrl(const QUrl& url) const
{
    static QString* userAgent = new QString(QSettings().value("UserAgent").toString());
    return !userAgent->isEmpty() ? *userAgent : QWebPage::userAgentForUrl(url);
}

QWebPage* WebOSWebPage::createWindow(WebWindowType type)
{
    if (QWebPage::WebBrowserWindow == type)
        return m_creator ? m_creator->createWebOSWebPage(type) : 0;

    return 0;
}

bool WebOSWebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type)
{
    if (!m_navigator)
        return QWebPage::acceptNavigationRequest(frame, request, type);

    if (QWebPage::NavigationTypeLinkClicked == type)
       return m_navigator->interceptLink(request.url()) ? false : true;

    return true;
}

void WebOSWebPage::javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID)
{
    qDebug("JS console: %s:%d - %s", qPrintable(sourceID), lineNumber, qPrintable(message));
}

WebOSWebPage::~WebOSWebPage()
{
    delete m_errorInfo;
}

