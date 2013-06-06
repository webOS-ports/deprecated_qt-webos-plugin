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

#include <QtGui/QWindowSystemInterface>
#include <QApplication>
#include <QSystemSemaphore>
#include <QElapsedTimer>
#include <QDebug>

#include "qweboswindow.h"
#include "qwebosscreen.h"
#include "qwebosglcontext.h"

#include <WebosSurfaceManagerClient.h>

#define MESSAGES_INTERNAL_FILE "SysMgrMessagesInternal.h"
#include <PIpcMessageMacros.h>
#include <PIpcChannel.h>

QT_BEGIN_NAMESPACE

#define Q_WEBOS_DEBUG 1

QWebOSWindow::QWebOSWindow(QWidget *widget, QWebOSScreen *screen)
    : QPlatformWindow(widget),
      OffscreenNativeWindow(widget->width(), widget->height()),
      m_screen(screen),
      m_glcontext(0),
      m_winid(-1),
      m_bufferSemaphore(0)
{
}

void QWebOSWindow::setGeometry(const QRect& rect)
{
    QWindowSystemInterface::handleGeometryChange(this->widget(), rect);

    // Since toplevels are fullscreen, propegate the screen size back to the widget
    widget()->setGeometry(rect);

    QPlatformWindow::setGeometry(rect);
}

void QWebOSWindow::setWinId(WId winId)
{
    m_winid = winId;
}

WId QWebOSWindow::winId() const
{
    return m_winid;
}

QPlatformGLContext *QWebOSWindow::glContext() const
{
    if (!m_glcontext)
        const_cast<QWebOSWindow*>(this)->createGLContext();

    return m_glcontext;
}

void QWebOSWindow::createGLContext()
{
    QPlatformWindowFormat format = widget()->platformWindowFormat();

    if (m_glcontext == 0 && format.windowApi() == QPlatformWindowFormat::OpenGL)
        m_glcontext = new QWebOSGLContext( const_cast<QWebOSWindow*>(this) );
}

void QWebOSWindow::waitForBuffer(OffscreenNativeWindowBuffer *buffer)
{
    if (m_winid == -1)
        return;

    if (!m_bufferSemaphore)
        m_bufferSemaphore = new QSystemSemaphore(QString("EGLWindow%1").arg(m_winid), 3, QSystemSemaphore::Create);

    m_bufferSemaphore->acquire();
}

void QWebOSWindow::postBuffer(OffscreenNativeWindowBuffer *buffer)
{
    // Only post buffer when we have assigned a valid window id as otherwise
    // the compositor can't associate the buffer with any active window
    if (m_winid != -1)
        m_surfaceClient.postBuffer(m_winid, buffer);
}

QT_END_NAMESPACE
