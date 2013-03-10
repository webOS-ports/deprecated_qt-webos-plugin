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

#include <QPlatformWindow>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

#include <EGL/eglhybris.h>

#include <SysMgrEvent.h>
#include <SysMgrDefs.h>
#include <SysMgrKeyEventTraits.h>
#include <SysMgrTouchEventTraits.h>

typedef WId QWebOSWindowId;

class HybrisCompositorClient;
class QWebOSScreen;
class QWebOSGLContext;
class QSystemSemaphore;
class PIpcChannel;
class QWebOSIpcClient;

class QWebOSWindow : public QPlatformWindow,
                     public OffscreenNativeWindow
{
public:
    QWebOSWindow(QWebOSIpcClient *ipcClient, HybrisCompositorClient *client, QWidget *w, QWebOSScreen *screen);

    virtual void setGeometry(const QRect &);
    WId winId() const;

    void setWinId(int winId) { m_winid = winId; }

    QPlatformGLContext *glContext() const;
    void setVisible(bool visible);

    virtual void postBuffer(OffscreenNativeWindowBuffer *buffer);

    void createGLContext();

public:
    void handleFocus(bool focused);
    void handleResize(int width, int height, bool resizeBuffer);
    void handleFullScreenEnabled();
    void handleFullScreenDisabled();
    void handlePause();
    void handleResume();
    void handleInputEvent(const SysMgrEventWrapper& wrapper);
    void handleTouchEvent(const SysMgrTouchEvent& touchEvent);
    void handleKeyEvent(const SysMgrKeyEvent& keyEvent);
    void handleBufferConsumed(int key);
    PIpcChannel* channel() const; // Required by IPC_MESSAGE_FORWARD

private:
    QWebOSScreen *m_screen;
    QWebOSGLContext *m_glcontext;
    WId m_winid;
    HybrisCompositorClient *m_client;
    QWebOSIpcClient *m_ipcClient;
    bool m_isWebAppMgr;
};

QT_END_NAMESPACE

#endif // QWEBOSWINDOW_H
