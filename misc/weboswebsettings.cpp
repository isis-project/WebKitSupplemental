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

#include "weboswebsettings.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtWebKit/qwebsettings.h>


namespace {

static bool localSettingsLoaded = false;

static const QString WEB_SETTINGS_KEY_PERSISTENT_STORAGE                    = QLatin1String("WebSettings/PersistentStorageEnabled");
static const QString WEB_SETTINGS_KEY_PERSISTENT_STORAGE_PATH               = QLatin1String("WebSettings/PersistentStoragePath");
static const QString WEB_SETTINGS_KEY_AUTO_LOAD_IMAGES                      = QLatin1String("WebSettings/AutoLoadImages");
static const QString WEB_SETTINGS_KEY_DNS_PREFETCH                          = QLatin1String("WebSettings/DnsPrefetchEnabled");
static const QString WEB_SETTINGS_KEY_ICON_DATABASE_PATH                    = QLatin1String("WebSettings/IconDatabasePath");
static const QString WEB_SETTINGS_KEY_JAVASCRIPT                            = QLatin1String("WebSettings/JavascriptEnabled");
static const QString WEB_SETTINGS_KEY_PLUGINS                               = QLatin1String("WebSettings/PluginsEnabled");
static const QString WEB_SETTINGS_KEY_PRIVATE_BROWSING                      = QLatin1String("WebSettings/PrivateBrowsingEnabled");
static const QString WEB_SETTINGS_KEY_JAVASCRIPT_CAN_OPEN_WINDOWS           = QLatin1String("WebSettings/JavascriptCanOpenWindows");
static const QString WEB_SETTINGS_KEY_JAVASCRIPT_CAN_CLOSE_WINDOWS          = QLatin1String("WebSettings/JavascriptCanCloseWindows");
static const QString WEB_SETTINGS_KEY_JAVASCRIPT_CAN_ACCESS_CLIPBOARD       = QLatin1String("WebSettings/JavascriptCanAccessClipboard");
static const QString WEB_SETTINGS_KEY_DEVELOPER_EXTRAS                      = QLatin1String("WebSettings/DeveloperExtrasEnabled");
static const QString WEB_SETTINGS_KEY_SPACIAL_NAVIGATION                    = QLatin1String("WebSettings/SpatialNavigationEnabled");
static const QString WEB_SETTINGS_KEY_LINKS_INCLUDED_IN_FOCUS_CHAIN         = QLatin1String("WebSettings/LinksIncludedInFocusChain");
static const QString WEB_SETTINGS_KEY_ZOOM_TEXT_ONLY                        = QLatin1String("WebSettings/ZoomTextOnly");
static const QString WEB_SETTINGS_KEY_PRINT_ELEMENT_BACKGROUNDS             = QLatin1String("WebSettings/PrintElementBackgrounds");
static const QString WEB_SETTINGS_KEY_OFFLINE_STORAGE_DATABASE              = QLatin1String("WebSettings/OfflineStorageDatabaseEnabled");
static const QString WEB_SETTINGS_KEY_OFFLINE_STORAGE_DATABASE_PATH         = QLatin1String("WebSettings/OfflineStoragePath");
static const QString WEB_SETTINGS_KEY_OFFLINE_STORAGE_QUOTA                 = QLatin1String("WebSettings/OfflineStorageDefaultQuota");
static const QString WEB_SETTINGS_KEY_OFFLINE_WEBAPP_CACHE                  = QLatin1String("WebSettings/OfflineWebApplicationCacheEnabled");
static const QString WEB_SETTINGS_KEY_OFFLINE_WEBAPP_CACHE_PATH             = QLatin1String("WebSettings/OfflineWebApplicationCachePath");
static const QString WEB_SETTINGS_KEY_OFFLINE_WEBAPP_QUOTA                  = QLatin1String("WebSettings/OfflineWebApplicationCacheQuota");
static const QString WEB_SETTINGS_KEY_LOCAL_STORAGE                         = QLatin1String("WebSettings/LocalStorageEnabled");
static const QString WEB_SETTINGS_KEY_LOCAL_STORAGE_PATH                    = QLatin1String("WebSettings/LocalStoragePath");
static const QString WEB_SETTINGS_KEY_LOCAL_CONTENT_CAN_ACCESS_REMOTE_URLS  = QLatin1String("WebSettings/LocalContentCanAccessRemoteUrls");
static const QString WEB_SETTINGS_KEY_LOCAL_CONTENT_CAN_ACCESS_FILE_URLS    = QLatin1String("WebSettings/LocalContentCanAccessFileUrls");
static const QString WEB_SETTINGS_KEY_XSS_AUDITING                          = QLatin1String("WebSettings/XSSAuditingEnabled");
static const QString WEB_SETTINGS_KEY_ACCELERATED_COMPOSITING               = QLatin1String("WebSettings/AcceleratedCompositingEnabled");
static const QString WEB_SETTINGS_KEY_WEBGL                                 = QLatin1String("WebSettings/WebGLEnabled");
static const QString WEB_SETTINGS_KEY_TILE_BACKING_STORE                    = QLatin1String("WebSettings/TiledBackingStoreEnabled");
static const QString WEB_SETTINGS_KEY_FRAME_FLATTENING                      = QLatin1String("WebSettings/FrameFlatteningEnabled");
static const QString WEB_SETTINGS_KEY_SITE_SPECIFIC_QUIRKS                  = QLatin1String("WebSettings/SiteSpecificQuirksEnabled");
static const QString WEB_SETTINGS_KEY_MAX_PAGES_IN_CACHE                    = QLatin1String("WebSettings/MaximumPagesInCache");
static const QString WEB_SETTINGS_KEY_OBJ_CACHE_CAPACITY_MIN                = QLatin1String("WebSettings/ObjectCacheCapacityMin");
static const QString WEB_SETTINGS_KEY_OBJ_CACHE_CAPACITY_MAX                = QLatin1String("WebSettings/ObjectCacheCapacityMax");
static const QString WEB_SETTINGS_KEY_OBJ_CACHE_CAPACITY_OVERALL            = QLatin1String("WebSettings/ObjectCacheCapacityOverall");
static const QString WEB_SETTINGS_KEY_PLUGIN_SUPPLEMENTAL_PATH              = QLatin1String("WebSettings/PluginSupplementalPath");
static const QString WEB_SETTINGS_KEY_PLUGIN_SUPPLEMENTAL_USER_PATH         = QLatin1String("WebSettings/PluginSupplementalUserPath");

}

namespace webOS {

bool WebSettings::initSettings(const QVariantMap &settingsMap)
{
    if (localSettingsLoaded)
        return false;

    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, QLatin1String("/var/tmp"));

    QString applicationName = QCoreApplication::applicationName();
    if (applicationName.isEmpty()) {
        applicationName = QLatin1String("WebSettings");
        QCoreApplication::setApplicationName(applicationName);
    }

    QString organizationName = QCoreApplication::organizationName();
    if (organizationName.isEmpty()) {
        organizationName = QLatin1String("palm");
        QCoreApplication::setOrganizationName(organizationName);
    }

    QString userScopeFile = QString("/var/tmp/%1/%2.conf").arg(organizationName).arg(applicationName);

    if (QFile::exists(userScopeFile))
        QFile::remove(userScopeFile);

    QString systemScopeFile = QString("/etc/palm/%1.conf").arg(applicationName);
    qDebug("webOS::WebSettings::initSettings: loading settings from: %s", qPrintable(systemScopeFile));

    QSettings systemSettings(systemScopeFile, QSettings::NativeFormat);
    QSettings settings;

    // add all values from settingsMap
    QVariantMap::const_iterator it;
    for (it = settingsMap.constBegin(); it != settingsMap.constEnd(); ++it)
        settings.setValue(it.key(), it.value());

    // override settingsMap values with system conf values
    QStringList keyList = systemSettings.allKeys();
    foreach (QString key, keyList)
        settings.setValue(key, systemSettings.value(key));

    localSettingsLoaded = true;
    return true;
}


bool WebSettings::initWebSettings()
{
    if (!localSettingsLoaded)
        return false;

    qDebug("webOS::WebSettings::initWebSettings");

    QWebSettings* globalWebSettings = QWebSettings::globalSettings();
    QSettings settings;

    if (settings.value(WEB_SETTINGS_KEY_PERSISTENT_STORAGE).toBool()) {
        QString path = settings.value(WEB_SETTINGS_KEY_PERSISTENT_STORAGE_PATH).toString();

        if (!path.isEmpty()) {
            globalWebSettings->setAttribute(QWebSettings::LocalStorageEnabled, true);
            globalWebSettings->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
            globalWebSettings->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
            globalWebSettings->setIconDatabasePath(path);
            globalWebSettings->setOfflineWebApplicationCachePath(path);
            globalWebSettings->setOfflineStoragePath(QString("%1/Databases").arg(path));
            globalWebSettings->setLocalStoragePath(QString("%1/LocalStorage").arg(path));
        }
    }

    if (settings.contains(WEB_SETTINGS_KEY_AUTO_LOAD_IMAGES))
        globalWebSettings->setAttribute(QWebSettings::AutoLoadImages, settings.value(WEB_SETTINGS_KEY_AUTO_LOAD_IMAGES).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_DNS_PREFETCH))
        globalWebSettings->setAttribute(QWebSettings::DnsPrefetchEnabled, settings.value(WEB_SETTINGS_KEY_DNS_PREFETCH).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_ICON_DATABASE_PATH))
        globalWebSettings->setIconDatabasePath(settings.value(WEB_SETTINGS_KEY_ICON_DATABASE_PATH).toString());

    if (settings.contains(WEB_SETTINGS_KEY_JAVASCRIPT))
        globalWebSettings->setAttribute(QWebSettings::JavascriptEnabled, settings.value(WEB_SETTINGS_KEY_JAVASCRIPT).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_PLUGINS))
        globalWebSettings->setAttribute(QWebSettings::PluginsEnabled, settings.value(WEB_SETTINGS_KEY_PLUGINS).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_PRIVATE_BROWSING))
        globalWebSettings->setAttribute(QWebSettings::PrivateBrowsingEnabled, settings.value(WEB_SETTINGS_KEY_PRIVATE_BROWSING).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_JAVASCRIPT_CAN_OPEN_WINDOWS))
        globalWebSettings->setAttribute(QWebSettings::JavascriptCanOpenWindows, settings.value(WEB_SETTINGS_KEY_JAVASCRIPT_CAN_OPEN_WINDOWS).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_JAVASCRIPT_CAN_CLOSE_WINDOWS))
        globalWebSettings->setAttribute(QWebSettings::JavascriptCanCloseWindows, settings.value(WEB_SETTINGS_KEY_JAVASCRIPT_CAN_CLOSE_WINDOWS).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_JAVASCRIPT_CAN_ACCESS_CLIPBOARD))
        globalWebSettings->setAttribute(QWebSettings::JavascriptCanAccessClipboard, settings.value(WEB_SETTINGS_KEY_JAVASCRIPT_CAN_ACCESS_CLIPBOARD).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_DEVELOPER_EXTRAS))
        globalWebSettings->setAttribute(QWebSettings::DeveloperExtrasEnabled, settings.value(WEB_SETTINGS_KEY_DEVELOPER_EXTRAS).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_SPACIAL_NAVIGATION))
        globalWebSettings->setAttribute(QWebSettings::SpatialNavigationEnabled, settings.value(WEB_SETTINGS_KEY_SPACIAL_NAVIGATION).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_LINKS_INCLUDED_IN_FOCUS_CHAIN))
        globalWebSettings->setAttribute(QWebSettings::LinksIncludedInFocusChain, settings.value(WEB_SETTINGS_KEY_LINKS_INCLUDED_IN_FOCUS_CHAIN).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_ZOOM_TEXT_ONLY))
        globalWebSettings->setAttribute(QWebSettings::ZoomTextOnly, settings.value(WEB_SETTINGS_KEY_ZOOM_TEXT_ONLY).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_PRINT_ELEMENT_BACKGROUNDS))
        globalWebSettings->setAttribute(QWebSettings::PrintElementBackgrounds, settings.value(WEB_SETTINGS_KEY_PRINT_ELEMENT_BACKGROUNDS).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_OFFLINE_STORAGE_DATABASE)) {
        bool offlineStorageDatabaseEnabled = settings.value(WEB_SETTINGS_KEY_OFFLINE_STORAGE_DATABASE).toBool();
        globalWebSettings->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, offlineStorageDatabaseEnabled);
        if (offlineStorageDatabaseEnabled && settings.contains(WEB_SETTINGS_KEY_OFFLINE_STORAGE_DATABASE_PATH))
            globalWebSettings->setOfflineStoragePath(settings.value(WEB_SETTINGS_KEY_OFFLINE_STORAGE_DATABASE_PATH).toString());
    }

    if (settings.contains(WEB_SETTINGS_KEY_OFFLINE_WEBAPP_CACHE)) {
        bool offlineWebApplicationCachedEnabled = settings.value(WEB_SETTINGS_KEY_OFFLINE_WEBAPP_CACHE).toBool();
        globalWebSettings->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, offlineWebApplicationCachedEnabled);
        if (offlineWebApplicationCachedEnabled && settings.contains(WEB_SETTINGS_KEY_OFFLINE_WEBAPP_CACHE_PATH))
            globalWebSettings->setOfflineWebApplicationCachePath(settings.value(WEB_SETTINGS_KEY_OFFLINE_WEBAPP_CACHE_PATH).toString());
    }

    if (settings.contains(WEB_SETTINGS_KEY_LOCAL_STORAGE)) {
        bool localStorageEnabled = settings.value(WEB_SETTINGS_KEY_LOCAL_STORAGE).toBool();
        globalWebSettings->setAttribute(QWebSettings::LocalStorageEnabled, localStorageEnabled);
        if (localStorageEnabled && settings.contains(WEB_SETTINGS_KEY_LOCAL_STORAGE_PATH))
            globalWebSettings->setLocalStoragePath(settings.value(WEB_SETTINGS_KEY_LOCAL_STORAGE_PATH).toString());
    }

    if (settings.contains(WEB_SETTINGS_KEY_LOCAL_CONTENT_CAN_ACCESS_REMOTE_URLS))
        globalWebSettings->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, settings.value(WEB_SETTINGS_KEY_LOCAL_CONTENT_CAN_ACCESS_REMOTE_URLS).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_LOCAL_CONTENT_CAN_ACCESS_FILE_URLS))
        globalWebSettings->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, settings.value(WEB_SETTINGS_KEY_LOCAL_CONTENT_CAN_ACCESS_FILE_URLS).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_XSS_AUDITING))
        globalWebSettings->setAttribute(QWebSettings::XSSAuditingEnabled, settings.value(WEB_SETTINGS_KEY_XSS_AUDITING).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_ACCELERATED_COMPOSITING))
        globalWebSettings->setAttribute(QWebSettings::AcceleratedCompositingEnabled, settings.value(WEB_SETTINGS_KEY_ACCELERATED_COMPOSITING).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_WEBGL))
        globalWebSettings->setAttribute(QWebSettings::WebGLEnabled, settings.value(WEB_SETTINGS_KEY_WEBGL).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_TILE_BACKING_STORE))
        globalWebSettings->setAttribute(QWebSettings::TiledBackingStoreEnabled, settings.value(WEB_SETTINGS_KEY_TILE_BACKING_STORE).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_FRAME_FLATTENING))
        globalWebSettings->setAttribute(QWebSettings::FrameFlatteningEnabled, settings.value(WEB_SETTINGS_KEY_FRAME_FLATTENING).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_SITE_SPECIFIC_QUIRKS))
        globalWebSettings->setAttribute(QWebSettings::SiteSpecificQuirksEnabled, settings.value(WEB_SETTINGS_KEY_SITE_SPECIFIC_QUIRKS).toBool());

    if (settings.contains(WEB_SETTINGS_KEY_MAX_PAGES_IN_CACHE))
        globalWebSettings->setMaximumPagesInCache(settings.value(WEB_SETTINGS_KEY_MAX_PAGES_IN_CACHE).toInt());

    if (settings.contains(WEB_SETTINGS_KEY_OBJ_CACHE_CAPACITY_MIN) 
            && settings.contains(WEB_SETTINGS_KEY_OBJ_CACHE_CAPACITY_MAX) 
            && settings.contains(WEB_SETTINGS_KEY_OBJ_CACHE_CAPACITY_OVERALL)) {
        globalWebSettings->setObjectCacheCapacities(
            stringToBytes(settings.value(WEB_SETTINGS_KEY_OBJ_CACHE_CAPACITY_MIN).toString()),
            stringToBytes(settings.value(WEB_SETTINGS_KEY_OBJ_CACHE_CAPACITY_MAX).toString()),
            stringToBytes(settings.value(WEB_SETTINGS_KEY_OBJ_CACHE_CAPACITY_OVERALL).toString()));
    }

    if (settings.contains(WEB_SETTINGS_KEY_OFFLINE_STORAGE_QUOTA))
        globalWebSettings->setOfflineStorageDefaultQuota(stringToBytes(settings.value(WEB_SETTINGS_KEY_OFFLINE_STORAGE_QUOTA).toString()));

    if (settings.contains(WEB_SETTINGS_KEY_OFFLINE_WEBAPP_QUOTA))
        globalWebSettings->setOfflineStorageDefaultQuota(stringToBytes(settings.value(WEB_SETTINGS_KEY_OFFLINE_WEBAPP_QUOTA).toString()));

    QStringList supplementalPath;

    if (settings.contains(WEB_SETTINGS_KEY_PLUGIN_SUPPLEMENTAL_PATH))
        supplementalPath.append(settings.value(WEB_SETTINGS_KEY_PLUGIN_SUPPLEMENTAL_PATH).toString());

    if (settings.contains(WEB_SETTINGS_KEY_PLUGIN_SUPPLEMENTAL_USER_PATH))
        supplementalPath.append(settings.value(WEB_SETTINGS_KEY_PLUGIN_SUPPLEMENTAL_USER_PATH).toString());

    if (!supplementalPath.isEmpty())
        globalWebSettings->setPluginSupplementalPath(supplementalPath.join(QLatin1String(":")));

    return true;
}

quint64 WebSettings::stringToBytes(const QString &srcString)
{
    quint64 bytes = 1;
    QString subString(srcString);

    if (srcString.contains(QChar('k'), Qt::CaseInsensitive) 
            || srcString.contains(QChar('m'), Qt::CaseInsensitive) 
            || srcString.contains(QChar('g'), Qt::CaseInsensitive)) {

        bytes = 1024;
        int pos = srcString.indexOf(QChar('k'), 0, Qt::CaseInsensitive);
        if (pos == -1) {
            bytes *= 1024;
            pos = srcString.indexOf(QChar('m'), 0, Qt::CaseInsensitive);
            if (pos == -1) {
                bytes *= 1024;
                pos = srcString.indexOf(QChar('g'), 0, Qt::CaseInsensitive);
            }
        }

        if (pos != -1)
            subString = srcString.left(pos);
    }

    return subString.toDouble() * bytes;
}

} // webOS
