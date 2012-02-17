WebKitSupplemental
==================

This is a supplemental component to webkit. The main goal of this component is to separate all
possible webOS specific webkit changes to be done outside webkit source directory.

Right now, This component has source code for:

* widgets: Contain custom Phrase picker widget for Spell Checker and Cut-Copy-Paste
* browserserver rendering using qbsplugin and qtwebkit plugins
* misc: contain extended QWebPage and WebSettings APIs

Even in future, Any webkit changes for webOS platform which cannot be contributed back to webkit.org
shall be done as part of this component.

##Licensing Information

###The license for WebKitSupplemental is Apache 2.0.
_However, where source files are derived from other external open source projects, the original license from those projects is maintained._

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

##Usage & APIs
This component has the supplemental features of webkit for webOS platform.

The below libraries shall be used to get the required features

* libWebKitMisc.a
* libWebKitWidgets.a
* libqbsplugin.so
* libqtwebkitplugin.so

Some of the features are designed only for Browser app and others are common
for browser and platform.

####setSelectionColors(const QGraphicsWebView*)
Define the color pallette to be used for active selection on webpage
####WebOSWebPage::WebOSWebPage()
extend QWebPage as WebOSWebPage to enable error handling and JS console messages
####initWebSettings() and initSettings()
APIs to store/retrive QSettings from configuration files defined for webOS
####PhrasePickerWidget and PickerPhrase classes
Custom Qt widget to display matching words as part of spell checker. In addition,
the same picker is used to display Cut-Copy-Paste widgets while selection
####qtwebkitplugin
This sub component has APIs defined for qt specific webkit extensions. touch modifier,
Spell checker are designed as qtwekit platform plugins.
####qbsplugin
browserserver plugin to handle rendering/window surface issues

##How to Build & Dependencies
WebKitSupplemental depends on Qt4.8 and QtWebKit.

WebKitSupplemental is built using the OpenEmbedded build system.
