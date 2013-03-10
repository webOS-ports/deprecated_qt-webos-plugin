/**
 *  Copyright (c) 2012 Hewlett-Packard Development Company, L.P.
 *                2013 Simon Busch <morphis@gravedo.de>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "qwebosintegration.h"
#include "qweboswindow.h"
#include "qwebosglcontext.h"
#include "qweboswindowsurface.h"

#include <QtGui/QPlatformGLContext>

#include <QtOpenGL/private/qgl_p.h>
#include <QtOpenGL/private/qglpaintdevice_p.h>
#include <QWindowSystemInterface>
#include <QDebug>
//#include <palmimedefines.h>

QT_BEGIN_NAMESPACE

class QWebOSPaintDevice : public QGLPaintDevice
{
public:
    QWebOSPaintDevice(QWebOSGLContext* platformGLContext)
        : m_platformGLContext(platformGLContext)
    {
        qDebug()<<__PRETTY_FUNCTION__;
        m_context = QGLContext::fromPlatformGLContext(m_platformGLContext);
    }

    QSize size() const {
        qDebug()<<__PRETTY_FUNCTION__;
        return m_platformGLContext->surfaceSize();
    }

    QGLContext* context() const {
        qDebug()<<__PRETTY_FUNCTION__;
        return m_context;
    }

    QPaintEngine *paintEngine() const {
        qDebug()<<__PRETTY_FUNCTION__;
        return qt_qgl_paint_engine();
    }

private:
    QWebOSGLContext* m_platformGLContext;
    QGLContext *m_context;
};

QWebOSGLWindowSurface::QWebOSGLWindowSurface(QWebOSScreen *screen, QWidget *window)
    : QWindowSurface(window)
{
    qDebug() << __PRETTY_FUNCTION__;

    m_platformGLContext = static_cast<QWebOSGLContext*>(window->platformWindow()->glContext());
    m_paintDevice = new QWebOSPaintDevice(m_platformGLContext);
    m_screen = screen;
}

void QWebOSGLWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(region);
    Q_UNUSED(offset);

    // QGraphicsView contains a QWidget for its frame, even if it is not visible. Any repaint
    // on that frame widget will cause an extra buffer swap, causing problems. This filters out
    // that swap.
//    if(qobject_cast<QGraphicsView*>(widget))
//        return;
    widget->platformWindow()->glContext()->swapBuffers();
}


QWebOSGLWindowSurface::~QWebOSGLWindowSurface()
{
}

void QWebOSGLWindowSurface::resize(const QSize &size)
{
    Q_UNUSED(size);
}
void QWebOSGLWindowSurface::beginPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

void QWebOSGLWindowSurface::endPaint(const QRegion &region)
{
    Q_UNUSED(region);
}

QT_END_NAMESPACE
