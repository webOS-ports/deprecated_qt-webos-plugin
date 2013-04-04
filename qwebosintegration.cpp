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

#include "qwebosipcclient.h"
#include "qweboswindow.h"
#include "qweboswindowsurface.h"
#include "qwebosglcontext.h"
#include "qwebosscreen.h"
#include "qwebosclipboard.h"
#include "qwebosfontdatabase.h"
#include "qwebosnativeinterface.h"

#include <HybrisCompositorClient.h>

#include <QDebug>
#include <QtGui/QPlatformWindow>
#include <QtGui/QPlatformWindowFormat>
#include <QtGui/private/qpixmap_raster_p.h>
#include <QtOpenGL/private/qpixmapdata_egl_p.h>
#include <QtOpenGL/private/qpixmapdata_gl_p.h>
#include <EGL/egl.h>

#include <glib.h>
#include <QApplication>

QT_BEGIN_NAMESPACE

QPlatformClipboard* QWebOSIntegration::clipboard() const
{
    return (QWebOSClipboard*) m_clipboard;
}

QWebOSIntegration::QWebOSIntegration()
    : m_fontDb(new QWebOSFontDatabase()),
      m_nativeInterface(new QWebOSNativeInterface()),
      m_primaryScreen(0),
      m_ipcClient(0)
{
    qDebug() << __PRETTY_FUNCTION__;

    m_clipboard = new QWebOSClipboard();

    m_primaryScreen = new QWebOSScreen();
    m_screens.append(m_primaryScreen);

    // Only create IPC client when we're not running in context of the webappmanager
    if (::getenv("QT_WEBOS_WEBAPPMGR") == 0) {
        qDebug() << __PRETTY_FUNCTION__ << "Not running in context of webappmanager";
        m_context = g_main_context_default();
        m_mainLoop = g_main_loop_new(m_context, TRUE);
        m_ipcClient = new QWebOSIpcClient(m_mainLoop);
        qDebug() << __PRETTY_FUNCTION__ << "Not running in context of webappmanager 2";
    }

    QWebOSGLContext::initialize(EGL_DEFAULT_DISPLAY);
}

QWebOSIntegration::~QWebOSIntegration()
{
    if (m_ipcClient)
        delete m_ipcClient;
}

bool QWebOSIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
        case ThreadedPixmaps:
            return true;
        default:
            return QPlatformIntegration::hasCapability(cap);
    }
}

QPixmapData *QWebOSIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    return new QGLPixmapData(type);
}

QPlatformWindow *QWebOSIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    qDebug() << __PRETTY_FUNCTION__;
    Q_UNUSED(winId);
    return new QWebOSWindow(m_ipcClient, &m_compositorClient, widget, m_primaryScreen);
}


QWindowSurface *QWebOSIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);

    if(widget->platformWindowFormat().windowApi() == QPlatformWindowFormat::OpenGL)
        return new QWebOSGLWindowSurface(m_primaryScreen, widget);

    qFatal("non-opengl windowsurface not implemented yet!");
    return 0;
}

QPlatformFontDatabase* QWebOSIntegration::fontDatabase() const
{
    return m_fontDb;
}

QPlatformNativeInterface* QWebOSIntegration::nativeInterface() const
{
    return m_nativeInterface;
}

QT_END_NAMESPACE
