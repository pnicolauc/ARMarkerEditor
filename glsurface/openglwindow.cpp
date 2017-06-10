#include "openglwindow.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>
#include <QDebug>
#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>


OpenGLWindow::OpenGLWindow(QScreen* screen )
    : QWindow(screen)
    , m_context(0)
    , m_timer(0)
{

}



OpenGLWindow::~OpenGLWindow()
{

}

void OpenGLWindow::initializeWindow(QOpenGLContext *glContext, int refreshRate)
{
    if (m_context) {
        qDebug() << "Window has already been initialized";
        return;
    }

    m_context = glContext;

    setSurfaceType(OpenGLSurface);
    setFormat( m_context->format() );
    resize( 800, 600 );
    create();

    connect( this, SIGNAL( widthChanged( int ) ), this, SLOT( _resizeGL() ) );
    connect( this, SIGNAL( heightChanged( int ) ), this, SLOT( _resizeGL() ) );
    connect( m_context, SIGNAL(aboutToBeDestroyed()), this, SLOT(_cleanupGL()), Qt::DirectConnection );

    _initializeGL();
    _resizeGL();

    m_timer = new QTimer;
    m_timer->setInterval(refreshRate);
    connect(m_timer, &QTimer::timeout, this, &OpenGLWindow::_paintGL);
    m_timer->start();
}

bool OpenGLWindow::isOpenGLES()
{
    return m_context->format().renderableType() == QSurfaceFormat::OpenGLES;
}

void OpenGLWindow::_initializeGL()
{
    m_context->makeCurrent( this );

    this->initializeGL();
}

void OpenGLWindow::_paintGL()
{
    if(!isExposed())
        return;

    m_context->makeCurrent( this );

    this->paintGL();

    m_context->swapBuffers( this );
}



void OpenGLWindow::_resizeGL()
{
    m_context->makeCurrent( this );

    this->resizeGL( width(), height() );
    _paintGL();
}

void OpenGLWindow::_cleanupGL()
{
    m_context->makeCurrent( this );

    this->cleanupGL();
}

void OpenGLWindow::_markerChanged(int index,Marker* marker){
    this->markerChanged(index,marker);
}
