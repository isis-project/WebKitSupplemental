WebKitSupplemental
==================

#Component Description

This is a supplemental component to webkit. The main goal of this component is to separate all
possible webOS specific webkit changes to be done outside webkit source directory.

Right now, This component has source code for:

* widgets: Contain custom Phrase picker widget for Spell Checker and Cut-Copy-Paste
* browserserver rendering using qbsplugin and qtwebkit plugins
* misc: contain extended QWebPage and WebSettings APIs

Even in future, Any webkit changes for webOS platform which cannot be contributed back to webkit.org
shall be done as part of this component.

#Licensing Information

###As the component has some features implemented by HP, Some are extended from existing components and some changes copied from other components, source code copyrighted  under multiple licenses.

Copyright (c) 2012 Hewlett-Packard Development Company, L.P.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.



Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.


Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
Copyright (C) 2012 Hewlett-Packard Development Company, L.P.
All rights reserved.

GNU Lesser General Public License Usage
This file may be used under the terms of the GNU Lesser General Public
License version 2.1 as published by the Free Software Foundation and
appearing in the file LICENSE.LGPL included in the packaging of this
file. Please review the following information to ensure the GNU Lesser
General Public License version 2.1 requirements will be met:
http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.


#Usage & APIs
This component has the supplemental features of webkit for webOS platform.

The below libraries shall be used to get the required features
* libWebKitMisc.a
* libWebKitWidgets.a
* libqbsplugin.so
* libqtwebkitplugin.so

Some of the features are designed only for Browser app and others are common
for browser and platform.

###setSelectionColors(const QGraphicsWebView*)
Define the color pallette to be used for active selection on webpage
###WebOSWebPage::WebOSWebPage()
extend QWebPage as WebOSWebPage to enable error handling and JS console messages
###initWebSettings() and initSettings()
APIs to store/retrive QSettings from configuration files defined for webOS
###PhrasePickerWidget and PickerPhrase classes
Custom Qt widget to display matching words as part of spell checker. In addition,
the same picker is used to display Cut-Copy-Paste widgets while selection
###qtwebkitplugin
This sub component has APIs defined for qt specific webkit extensions. touch modifier,
Spell checker are designed as qtwekit platform plugins.
###qbsplugin
browserserver plugin to handle rendering/window surface issues

#How to Build & Dependencies
This component has the build dependency with Qt4.8 and qtwebkit
Right now, WebKitSupplemental is built using the OpenEmbedded build system.
