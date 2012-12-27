/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** Copyright (C) 2012 Hewlett-Packard Development Company, L.P.
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef qbsintegration_h
#define qbsintegration_h

#if QT_VERSION < 0x050000
#include <QtGui/QPlatformIntegration>
#include <QtGui/QPlatformScreen>
#else
#include <qpa/qplatformintegration.h>
#include <qpa/qplatformscreen.h>
#endif

QT_BEGIN_NAMESPACE

class QBsScreen : public QPlatformScreen {
public:
    QBsScreen()
        : mDepth(32), mFormat(QImage::Format_ARGB32_Premultiplied) { }

    QRect geometry() const { return mGeometry; }
    int depth() const { return mDepth; }
    QImage::Format format() const { return mFormat; }

public:
    QRect mGeometry;
    int mDepth;
    QImage::Format mFormat;
    QSize mPhysicalSize;
};

class QBsIntegration : public QPlatformIntegration {
public:
    QBsIntegration();

    bool hasCapability(QPlatformIntegration::Capability) const;

    QPixmapData* createPixmapData(QPixmapData::PixelType) const;
    QPlatformWindow* createPlatformWindow(QWidget*, WId winId) const;
    QWindowSurface* createWindowSurface(QWidget*, WId winId) const;
    QPlatformFontDatabase* fontDatabase() const;
    QList<QPlatformScreen *> screens() const { return mScreens; }

private:
    QList<QPlatformScreen *> mScreens;
    QPlatformFontDatabase *mFontDb;
};

QT_END_NAMESPACE

#endif
