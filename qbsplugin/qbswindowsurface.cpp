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

#include "qbswindowsurface.h"
#include <QtCore/qdebug.h>
#include <QtGui/private/qapplication_p.h>

extern "C" {

QBsDriver* qpa_qbs_register_client(QWidget *window, QBsClient*) __attribute__((visibility("default")));

QBsDriver* qpa_qbs_register_client(QWidget *window, QBsClient* client)
{
    return QBsWindowSurface::registerClient(window, client);
}

};

static void copyRegion(unsigned char* target, QSize targetSize, const unsigned char* source, QSize sourceSize, const QRegion &region)
{
    static const int pixelWidth = sizeof(unsigned int);

    QRect rect = region.boundingRect();

    int startX = rect.x();
    int startY = rect.y();
    int width = rect.width();
    int height = rect.height();
    int targetWidth = targetSize.width();
    int sourceWidth = sourceSize.width();

    if ((startX + width) > targetWidth)
        width = targetWidth - startX;

    if ((startX + width) > sourceWidth)
        width = sourceWidth - startX;

    if ((startY + height) > targetSize.height())
        height = targetSize.height() - startY;

    if ((startY + height) > sourceSize.height())
        height = sourceSize.height() - startY;

    if ((height > 0) && (width > 0)) {

       width *= pixelWidth;
       targetWidth *= pixelWidth;
       sourceWidth *= pixelWidth;
       int targetOffset = (startY * targetWidth) + (startX * pixelWidth);
       int sourceOffset = (startY * sourceWidth) + (startX * pixelWidth);

       for (int yy = 0; yy < height; yy++)
           memcpy(target + targetOffset + (yy * targetWidth), source + sourceOffset + (yy * sourceWidth), width);
    }
}

QT_BEGIN_NAMESPACE

QMap<QWidget *, QBsWindowSurface *> QBsWindowSurface::s_map;

QBsDriver* QBsWindowSurface::registerClient(QWidget *window, QBsClient* client)
{
    QBsWindowSurface* surface = s_map.value(window);

    return surface ? surface->registerClient(client) : 0;
}

QBsWindowSurface::QBsWindowSurface(QWidget *window)
    : QWindowSurface(window)
    , m_scratch(640, 480, QImage::Format_ARGB32_Premultiplied)
    , m_paintToScratch(false)
    , m_bufferActive0(false)
    , m_buffer0(0)
    , m_bufferLength0(0)
    , m_bufferActive1(false)
    , m_buffer1(0)
    , m_bufferLength1(0)
    , m_client(0)
{
    m_surface = m_scratch;
    s_map.insert(window, this);
}

QBsWindowSurface::~QBsWindowSurface()
{
    s_map.remove(window());
}

QBsDriver* QBsWindowSurface::registerClient(QBsClient* client)
{
    m_client = client;
    return this;
}

void QBsWindowSurface::updateBuffer(const QSize size, const QRegion &region, unsigned char* target, const unsigned char* source, QRegion &sourceRegion)
{
    if (m_paintToScratch && (!m_scratchRegion.subtract(region).isEmpty())) {

        copyRegion(target, size, m_scratch.constBits(), m_scratch.size(), m_scratchRegion);
        m_paintToScratch = false;
        m_scratchRegion = QRegion();
    }

    if (!sourceRegion.subtract(region).isEmpty()) {
    // qDebug() << "Copy region:" << sourceRegion.boundingRect() << "not in" << region.boundingRect();
        copyRegion(target, size, source, size, sourceRegion);
    }
}

QPaintDevice* QBsWindowSurface::paintDevice()
{
    // qDebug() << "QBsWindowSurface::paintDevice()";
    return &m_surface;
}

void QBsWindowSurface::beginPaint(const QRegion &region)
{
    // qDebug() << "QBsWindowSurface::beginPaint";
    if (m_bufferActive0 && m_buffer0 && m_buffer1) {

        m_surface = QImage(m_buffer0, size().width(), size().height(), QImage::Format_ARGB32_Premultiplied);

        updateBuffer(size(), region, m_buffer0, m_buffer1, m_region1);
        m_region0 = region;
    } else if (m_bufferActive1 && m_buffer0 && m_buffer1) {

        m_surface = QImage(m_buffer1, size().width(), size().height(), QImage::Format_ARGB32_Premultiplied);

        updateBuffer(size(), region, m_buffer1, m_buffer0, m_region0);
        m_region1 = region;
    } else {

        m_surface = m_scratch;

        m_paintToScratch = true;
        m_scratchRegion += region;
    }

    QWindowSurface::beginPaint(region);
}

void QBsWindowSurface::endPaint(const QRegion &region)
{
    // qDebug() << "QBsWindowSurface::endPaint";
    QWindowSurface::endPaint(region);
}

void QBsWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    // qDebug() << "QBsWindowSurface::flush";
    Q_UNUSED(widget);
    Q_UNUSED(region);
    Q_UNUSED(offset);

    if (m_client) {

        if (m_bufferActive0)
            m_client->flushBuffer(0);
        else if (m_bufferActive1)
            m_client->flushBuffer(1);
    } else
       qDebug() << " *** ERROR: NO Client";
}

void QBsWindowSurface::resize(const QSize &size)
{
    // qDebug() << "QBsWindowSurface::resize:" << size;
    QWindowSurface::resize(size);

    m_scratch = QImage(size, QImage::Format_ARGB32_Premultiplied);
}

void QBsWindowSurface::setBuffers(unsigned char* buffer0, int length0, unsigned char* buffer1, int length1)
{
    // qDebug() << "QBsWindowSurface::setBuffers()";
    m_buffer0 = buffer0;
    m_bufferLength0 = length0;
    m_bufferActive0 = false;

    m_buffer1 = buffer1;
    m_bufferLength1 = length1;
    m_bufferActive1 = false;

    if (!m_buffer0 || !m_buffer1)
        m_surface = m_scratch;
}

void QBsWindowSurface::setBufferState(int buffer, bool active)
{
    // qDebug() << "QBsWindowSurface::setBufferState(" << buffer << (active ? "TRUE" : "FALSE") << ")";
    if (buffer == (int)0)
        m_bufferActive0 = active;
    else if (buffer == 1)
        m_bufferActive1 = active;
}

void QBsWindowSurface::releaseBuffers()
{
    // qDebug() << "QBsWindowSurface::releaseBuffers()";
    setBuffers(0, 0, 0, 0);
}

QT_END_NAMESPACE
