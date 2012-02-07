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

#ifndef weboswebsettings_h
#define weboswebsettings_h

#include <QtCore/QVariant>


namespace webOS {

namespace WebSettings {

/**
 * Initialize QSettings to contain values from passed in settingsMap
 * Read in override settings from /etc/palm/ApplicationName.conf
 * QSettings userScope will be set to /var/tmp
 */
bool initSettings(const QVariantMap &settingsMap);

/**
 * Initialize QWebSettings with defaults from QSettings if they have been set
 */
bool initWebSettings();

/**
 * Expands "1MB" --> 1048576, "2k" --> 2048 etc.
 */
quint64 stringToBytes(const QString &srcString);

} // WebSettings

} // webOS

#endif // weboswebsettings_h
