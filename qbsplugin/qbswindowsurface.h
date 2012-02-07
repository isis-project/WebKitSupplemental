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

#ifndef qbswindowsurface_h
#define qbswindowsurface_h

#include "qbsinterface.h"

#include <QtGui/QPlatformWindow>
#include <QtGui/private/qwindowsurface_p.h>


QT_BEGIN_NAMESPACE

class QBsWindowSurface : public QWindowSurface, public QBsDriver {
public:
    static QBsDriver* registerClient(QWidget *window, QBsClient*);
    QBsWindowSurface(QWidget *window);
    ~QBsWindowSurface();

    QBsDriver* registerClient(QBsClient*);
    void updateBuffer(const QSize, const QRegion&, unsigned char* target, const unsigned char* source, QRegion &sourceRegion);

    QPaintDevice *paintDevice();
    virtual void beginPaint(const QRegion &);
    virtual void endPaint(const QRegion &);
    void flush(QWidget*, const QRegion&, const QPoint &offset);
    void resize(const QSize&);

    // QBsDriver Interface
    virtual void setBuffers(unsigned char* buffer0, int length0, unsigned char* buffer1, int length1);
    virtual void setBufferState(int buffer, bool active);
    virtual void releaseBuffers();

private:
    static QMap<QWidget*, QBsWindowSurface*> s_map;
    QImage m_surface;
    QImage m_scratch;
    QRegion m_scratchRegion;
    bool m_paintToScratch;
    bool m_bufferActive0;
    unsigned char* m_buffer0;
    int m_bufferLength0;
    QRegion m_region0;
    bool m_bufferActive1;
    unsigned char* m_buffer1;
    int m_bufferLength1;
    QRegion m_region1;
    QBsClient* m_client;
};

QT_END_NAMESPACE

#endif
