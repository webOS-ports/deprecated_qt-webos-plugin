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
#include <QElapsedTimer>
#include <QDebug>

#include <EGL/egl.h>

#include "qweboswindow.h"
#include "qwebosscreen.h"
#include "qwebosglcontext.h"

#include "WebosSurfaceManagerClient.h"

QT_BEGIN_NAMESPACE

#define Q_WEBOS_DEBUG 1

QWebOSWindow::QWebOSWindow(QWidget *widget, QWebOSScreen *screen)
    : QPlatformWindow(widget),
      m_screen(screen),
      m_glcontext(0),
      m_winid(-1),
      m_webosEglWindow(0)
{
    WebosSurfaceManagerClient::CreateNativeWindow(m_webosEglWindow);
    if(m_webosEglWindow)
        m_webosEglWindow->resize(widget->width(), widget->height());
}

void QWebOSWindow::setGeometry(const QRect& rect)
{
    QWindowSystemInterface::handleGeometryChange(this->widget(), rect);

    // Since toplevels are fullscreen, propegate the screen size back to the widget
    widget()->setGeometry(rect);

    QPlatformWindow::setGeometry(rect);
}

void QWebOSWindow::resize(int iNewWidth, int iNewHeight)
{
    /* Set the size of the window */
    if(m_webosEglWindow)
        m_webosEglWindow->resize(iNewWidth, iNewHeight);
}

void QWebOSWindow::setWinId(WId winId)
{
    m_winid = winId;

    if(m_webosEglWindow)
        m_webosEglWindow->identify(winId);
}

WId QWebOSWindow::winId() const
{
    return m_winid;
}

EGLNativeWindowType QWebOSWindow::getEglWindow()
{
    return (EGLNativeWindowType)m_webosEglWindow;
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

QT_END_NAMESPACE
