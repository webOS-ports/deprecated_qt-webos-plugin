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

#include "qwebosipcclient.h"

QT_BEGIN_NAMESPACE

#define Q_WEBOS_DEBUG 1

QWebOSWindow::QWebOSWindow(QWebOSIpcClient *ipcClient, WebosSurfaceManagerClient *client, QWidget *widget, QWebOSScreen *screen)
    : QPlatformWindow(widget),
      OffscreenNativeWindow(widget->width(), widget->height()),
      m_screen(screen),
      m_glcontext(0),
      m_client(client),
      m_ipcClient(ipcClient),
      m_winid(-1),
      m_isWebAppMgr(false),
      m_bufferSemaphore(0)
{
    m_isWebAppMgr = (::getenv("QT_WEBOS_WEBAPPMGR") != 0);
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

    if (!m_bufferSemaphore) {
        m_bufferSemaphore = new QSystemSemaphore(QString("EGLWindow%1").arg(m_winid), 3, QSystemSemaphore::Create);
    }

    m_bufferSemaphore->acquire();
}

void QWebOSWindow::postBuffer(OffscreenNativeWindowBuffer *buffer)
{
    // Only post buffer when we have assigned a valid window id as otherwise
    // the compositor can't associate the buffer with any active window
    if (m_winid != -1)
        m_client->postBuffer(m_winid, buffer);
}

void QWebOSWindow::handleFocus(bool focused)
{
    if (focused)
        QWindowSystemInterface::handleWindowActivated(widget());
    else
        QWindowSystemInterface::handleWindowActivated(0);
}

void QWebOSWindow::handleResize(int width, int height, bool resizeBuffer)
{
    qDebug() << __PRETTY_FUNCTION__ << "width =" << width << "height =" << height;
    Q_UNUSED(resizeBuffer)
    setGeometry(QRect(0, 0, width, height));
}

void QWebOSWindow::handleFullScreenEnabled()
{
}

void QWebOSWindow::handleFullScreenDisabled()
{
}

void QWebOSWindow::handlePause()
{
}

void QWebOSWindow::handleResume()
{
}

void QWebOSWindow::handleInputEvent(const SysMgrEventWrapper& wrapper)
{
    SysMgrEvent* e = wrapper.event;
    QPoint mousePos;

    switch(e->type)
    {
        case SysMgrEvent::Accelerometer:
            break;
        case SysMgrEvent::PenFlick:
            mousePos = QPoint(e->x, e->y);
            QWindowSystemInterface::handleWheelEvent(widget(),mousePos,mousePos,e->z,Qt::Vertical);
            break;
        case SysMgrEvent::PenPressAndHold:
            mousePos = QPoint(e->x, e->y);
            QWindowSystemInterface::handleMouseEvent(widget(), mousePos, mousePos, Qt::NoButton);
            break;
        case SysMgrEvent::PenDown:
            mousePos = QPoint(e->x, e->y);
            QWindowSystemInterface::handleMouseEvent(widget(), mousePos, mousePos, Qt::LeftButton);
            break;
        case SysMgrEvent::PenUp:
            mousePos = QPoint(e->x, e->y);
            QWindowSystemInterface::handleMouseEvent(widget(), mousePos, mousePos, Qt::NoButton);
            break;
        case SysMgrEvent::PenMove:
            mousePos = QPoint(e->x, e->y);
            QWindowSystemInterface::handleMouseEvent(widget(), mousePos, mousePos, Qt::LeftButton);
            break;
        case SysMgrEvent::GestureStart:
            break;
        case SysMgrEvent::GestureEnd:
            break;
        case SysMgrEvent::GestureCancel:
            break;
        default:
            break;
    }
}

void QWebOSWindow::handleTouchEvent(const SysMgrTouchEvent& touchEvent)
{
    QEvent::Type type = QEvent::None;

    QList<QWindowSystemInterface::TouchPoint> touchPoints;
    for (unsigned int i = 0; i < touchEvent.numTouchPoints; i++) {
        QWindowSystemInterface::TouchPoint touchPoint;
        QPoint pt(touchEvent.touchPoints[i].x, touchEvent.touchPoints[i].y);

        // get size of screen which contains window
        QPlatformScreen *platformScreen = QPlatformScreen::platformScreenForWidget(widget());
        QSizeF screenSize = platformScreen->physicalSize();

        touchPoint.id = touchEvent.touchPoints[i].id;

        // update cached position of current touch point
        touchPoint.normalPosition = QPointF( static_cast<qreal>(pt.x()) / screenSize.width(), static_cast<qreal>(pt.y()) / screenSize.height() );
        touchPoint.area = QRectF( pt.x(), pt.y(), 0.0, 0.0 );
        touchPoint.pressure = 1;

        touchPoint.state = static_cast<Qt::TouchPointState>(touchEvent.touchPoints[i].state);

        // FIXME: what if the touchpoints have different states? does this ever happen?
        switch (touchPoint.state) {
            case Qt::TouchPointPressed:
                type = QEvent::TouchBegin;
                break;
            case Qt::TouchPointMoved:
                type = QEvent::TouchUpdate;
                break;
            case Qt::TouchPointReleased:
                type = QEvent::TouchEnd;
                break;
        }

        touchPoints.append(touchPoint);

        QWindowSystemInterface::handleMouseEvent(widget(), pt, pt, (touchPoint.state != Qt::TouchPointReleased ? Qt::LeftButton : Qt::NoButton));
    }

    QWindowSystemInterface::handleTouchEvent(widget(), type, QTouchEvent::TouchScreen, touchPoints);
}

void QWebOSWindow::handleKeyEvent(const SysMgrKeyEvent& keyEvent)
{
    QKeyEvent ev = keyEvent.qtEvent();
    Qt::Key key;
    if (ev.key() == 0x01200001)
        key = Qt::Key_Backspace;
    else
        key = (Qt::Key)ev.key();
    QWindowSystemInterface::handleKeyEvent(widget(), ev.type(), key, ev.modifiers(), ev.text(), 0, 0);
}

PIpcChannel* QWebOSWindow::channel() const
{
    return m_ipcClient->channel();
}

void QWebOSWindow::setVisible(bool visible)
{
    qDebug() << __PRETTY_FUNCTION__;

    if (!m_isWebAppMgr) {
        if(!channel()) {
            qWarning() << "not connected to window manager...";
            return;
        }

        if (visible) {
            QSize size = geometry().size();
            channel()->sendSyncMessage(new ViewHost_PrepareAddWindow((1 << 1),
                size.width(), size.height(), &m_winid));

            m_ipcClient->addWindow(this);

            std::string winProps = "{ "
                    " 'fullScreen': false, " // defaults to false
                    " 'overlayNotificationsPosition': 'bottom', " // options are left, right, top, bottom
                    " 'subtleLightbar': true, " // defaults to false
                    " 'blockScreenTimeout': true, " // defaults to false
                    " 'fastAccelerometer': true, " // defaults to false
                    " 'suppressBannerMessages': false, " // defaults to false
                    " 'hasPauseUi': true " // defaults to false
                    " }";
            channel()->sendAsyncMessage(new ViewHost_SetWindowProperties(winId(), winProps));
            channel()->sendAsyncMessage(new ViewHost_AddWindow(winId()));
            channel()->sendAsyncMessage(new ViewHost_FocusWindow(winId()));

            setGeometry(QRect(QPoint(), size));
        }
    }

    QPlatformWindow::setVisible(visible);
}

QT_END_NAMESPACE
