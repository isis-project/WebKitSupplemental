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

#include "webosmisc.h"

#include <QColor>
#include <QtCore/QSettings>

namespace webOS {

bool setSelectionColors(const QGraphicsWebView* view)
{
    if (!view)
        return false;

    QSettings settings;
    QColor selectionColor = QColor(settings.value("SelectionColor").toString());
    QColor highlightedTextColor = QColor(settings.value("HighlightedTextColor").toString());

    QPalette palette = view->palette();
    palette.setBrush(QPalette::Active, QPalette::Highlight, selectionColor);
    palette.setBrush(QPalette::Active, QPalette::HighlightedText, highlightedTextColor);
    view->setPalette(palette);
    return true;
}

} // webOS
