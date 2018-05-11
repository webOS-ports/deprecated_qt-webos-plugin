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

#ifndef QWEBOSWINDOW_H
#define QWEBOSWINDOW_H

#include <QSystemSemaphore>
#include <QPlatformWindow>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

#include <EGL/egl.h>

typedef WId QWebOSWindowId;

class QWebOSScreen;
class QWebOSGLContext;
class IWebosEglWindow;

class QWebOSWindow : public QPlatformWindow
{
public:
    QWebOSWindow(QWidget *w, QWebOSScreen *screen);

    virtual void setGeometry(const QRect &);

    void resize(int iNewWidth, int iNewHeight);

    WId winId() const;
    virtual void setWinId(WId winId);

    QPlatformGLContext *glContext() const;

    EGLNativeWindowType getEglWindow();

    void createGLContext();

protected:
    virtual unsigned int platformWindowId() { return m_winid; }

private:
    QWebOSScreen *m_screen;
    QWebOSGLContext *m_glcontext;
    WId m_winid;
    IWebosEglWindow *m_webosEglWindow;
};

QT_END_NAMESPACE

#endif // QWEBOSWINDOW_H
