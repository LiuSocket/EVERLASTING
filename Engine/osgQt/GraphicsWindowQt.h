/* -*-c++-*- OpenSceneGraph - Copyright (C) 2009 Wang Rui
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/
#pragma once

#include <QGLWidget>
#include <osgViewer/GraphicsWindow>

#include <QMutex>
#include <QEvent>
#include <QQueue>
#include <QSet>

class QInputEvent;
class QGestureEvent;

namespace osgViewer {
    class ViewerBase;
}

namespace osgQt
{

// forward declarations
class GraphicsWindowQt;

/// The function sets the WindowingSystem to Qt.
void initQtWindowingSystem();

/** The function sets the viewer that will be used after entering
 *  the Qt main loop (QCoreApplication::exec()).
 *
 *  The function also initializes internal structures required for proper
 *  scene rendering.
 *
 *  The method must be called from main thread. */
void setViewer( osgViewer::ViewerBase *viewer );


class GLWidget : public QGLWidget
{
    typedef QGLWidget inherited;

public:

    GLWidget( QWidget* parent = NULL, const QGLWidget* shareWidget = NULL, Qt::WindowFlags f = 0, bool forwardKeyEvents = false );
    GLWidget( QGLContext* context, QWidget* parent = NULL, const QGLWidget* shareWidget = NULL, Qt::WindowFlags f = 0, bool forwardKeyEvents = false );
    GLWidget( const QGLFormat& format, QWidget* parent = NULL, const QGLWidget* shareWidget = NULL, Qt::WindowFlags f = 0, bool forwardKeyEvents = false );
    virtual ~GLWidget();

    inline void setGraphicsWindow( GraphicsWindowQt* gw ) { _gw = gw; }
    inline GraphicsWindowQt* getGraphicsWindow() { return _gw; }
    inline const GraphicsWindowQt* getGraphicsWindow() const { return _gw; }

    inline bool getForwardKeyEvents() const { return _forwardKeyEvents; }
    virtual void setForwardKeyEvents( bool f ) { _forwardKeyEvents = f; }
    
    inline bool getTouchEventsEnabled() const { return _touchEventsEnabled; }
    void setTouchEventsEnabled( bool e );

    void setKeyboardModifiers( QInputEvent* event );

    virtual void keyPressEvent( QKeyEvent* event );
    virtual void keyReleaseEvent( QKeyEvent* event );
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseDoubleClickEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void wheelEvent( QWheelEvent* event );
    virtual bool gestureEvent( QGestureEvent* event );

protected:

    int getNumDeferredEvents()
    {
        QMutexLocker lock(&_deferredEventQueueMutex);
        return _deferredEventQueue.count();
    }
    void enqueueDeferredEvent(QEvent::Type eventType, QEvent::Type removeEventType = QEvent::None)
    {
        QMutexLocker lock(&_deferredEventQueueMutex);

        if (removeEventType != QEvent::None)
        {
            if (_deferredEventQueue.removeOne(removeEventType))
                _eventCompressor.remove(eventType);
        }

        if (_eventCompressor.find(eventType) == _eventCompressor.end())
        {
            _deferredEventQueue.enqueue(eventType);
            _eventCompressor.insert(eventType);
        }
    }
    void processDeferredEvents();

    friend class GraphicsWindowQt;
    GraphicsWindowQt* _gw;

    QMutex _deferredEventQueueMutex;
    QQueue<QEvent::Type> _deferredEventQueue;
    QSet<QEvent::Type> _eventCompressor;

    bool _touchEventsEnabled;

    bool _forwardKeyEvents;
    qreal _devicePixelRatio;

    virtual void resizeEvent( QResizeEvent* event );
    virtual void moveEvent( QMoveEvent* event );
    virtual void glDraw();
    virtual bool event( QEvent* event );
};

class GraphicsWindowQt : public osgViewer::GraphicsWindow
{
public:
    GraphicsWindowQt( osg::GraphicsContext::Traits* traits, QWidget* parent = NULL, const QGLWidget* shareWidget = NULL, Qt::WindowFlags f = 0 );
    GraphicsWindowQt( GLWidget* widget );
    virtual ~GraphicsWindowQt();

    inline GLWidget* getGLWidget() { return _widget; }
    inline const GLWidget* getGLWidget() const { return _widget; }

    struct WindowData : public osg::Referenced
    {
        WindowData( GLWidget* widget = NULL, QWidget* parent = NULL ): _widget(widget), _parent(parent) {}
        GLWidget* _widget;
        QWidget* _parent;
    };

    bool init( QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags f );

    static QGLFormat traits2qglFormat( const osg::GraphicsContext::Traits* traits );
    static void qglFormat2traits( const QGLFormat& format, osg::GraphicsContext::Traits* traits );
    static osg::GraphicsContext::Traits* createTraits( const QGLWidget* widget );

    virtual bool setWindowRectangleImplementation( int x, int y, int width, int height );
    virtual void getWindowRectangle( int& x, int& y, int& width, int& height );
    virtual bool setWindowDecorationImplementation( bool windowDecoration );
    virtual bool getWindowDecoration() const;
    virtual void grabFocus();
    virtual void grabFocusIfPointerInWindow();
    virtual void raiseWindow();
    virtual void setWindowName( const std::string& name );
    virtual std::string getWindowName();
    virtual void useCursor( bool cursorOn );
    virtual void setCursor( MouseCursor cursor );
    inline bool getTouchEventsEnabled() const { return _widget->getTouchEventsEnabled(); }
    virtual void setTouchEventsEnabled( bool e ) { _widget->setTouchEventsEnabled(e); }


    virtual bool valid() const;
    virtual bool realizeImplementation();
    virtual bool isRealizedImplementation() const;
    virtual void closeImplementation();
    virtual bool makeCurrentImplementation();
    virtual bool releaseContextImplementation();
    virtual void swapBuffersImplementation();
    virtual void runOperations();

    virtual void requestWarpPointer( float x, float y );

protected:

    friend class GLWidget;
    GLWidget* _widget;
    bool _ownsWidget;
    QCursor _currentCursor;
    bool _realized;
};

} // namespace osgQt
