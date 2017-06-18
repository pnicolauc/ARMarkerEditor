#include <QQuickView>
#include <QWidget>
#include <QFileDialog>
#include <menu/mainwindow.h>
#include <QString>
#include <glsurface/openglwindow.h>
#include <glsurface/modelwindow_gl.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <menu/markermenu.h>
#include <menu/cameramenu.h>

#include <QOpenGLContext>
#include <glsurface/glsignalemitter.h>

QOpenGLContext *createOpenGLContext(int major, int minor) {
    QSurfaceFormat requestedFormat;
    requestedFormat.setDepthBufferSize( 24 );
    requestedFormat.setMajorVersion( major );
    requestedFormat.setMinorVersion( minor );

    requestedFormat.setSamples( 4 );
    requestedFormat.setProfile( QSurfaceFormat::CoreProfile );

    QOpenGLContext *context = new QOpenGLContext;
    context->setFormat( requestedFormat );
    context->create();

    // If unable to get OpenGL 3.3 context, fall back to OpenGL 2.1/OpenGL ES
    if(context->format().version() != qMakePair(major,minor)) {
        qDebug() << "Unable to get OpenGL context version" << QString(QString::number(major) + "." + QString::number(minor));
        context->deleteLater();
        requestedFormat.setMajorVersion(2);
        requestedFormat.setMinorVersion(1);
        context = new QOpenGLContext;
        context->setFormat( requestedFormat );
        context->create();

        // OpenGL versions < 2 is not supported
        if (context->format().version().first < 2) {
            context->deleteLater();
            context = 0;
        }
    }

    return context;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui->actionOpen_Model_2,SIGNAL(triggered()),this,SLOT(addGLWindow()));
    this->setFixedSize(970,550);


}

void MainWindow::addGLWindow(){
    QUrl sourceFileName;
    QWidget *container = new QWidget();
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    sourceFileName = dialog.getOpenFileUrl(container, QStringLiteral("Open a scene file"));
    if (sourceFileName.isEmpty())
        qApp->quit();

    QOpenGLContext *context = createOpenGLContext(3, 3);

    if (!context)
        qApp->quit();

    QPair<int, int> glVersion = context->format().version();
    bool isOpenGLES = context->format().renderableType() == QSurfaceFormat::OpenGLES;

    qDebug() << "OpenGL Version:" << glVersion << (isOpenGLES ? "ES" : "Desktop");

    //CHANGE PROJECT
    //TODO - add dialog to make sure of replacing gl window
    if (glVersion == qMakePair(3,3) && !isOpenGLES) {
        modelWindow = new ModelWindow_GL(sourceFileName.path(), ModelLoader::RelativePath);//new Scene(OpenGL_Model, ModelLoader::RelativePath);
    }
    modelWindow->initializeWindow(context, 40);

    QWidget *glcontainer = QWidget::createWindowContainer(modelWindow, this);
    glcontainer->setMinimumSize(500, 500);
    glcontainer->setMaximumSize(1000, 1000);
    glcontainer->setFocusPolicy(Qt::TabFocus);
    ui->glContainer->addWidget(glcontainer);

    setupGLSignals();

}


void MainWindow::setupGLSignals(){
    connect(modelWindow->entities.glSignalEmitter,SIGNAL(editMarker(int,Marker*)), this, SLOT(addMarkerMenu(int,Marker*)));
    connect(modelWindow->entities.glSignalEmitter,SIGNAL(editCamera(int,Camera*,bool*)), this, SLOT(addCameraMenu(int,Camera*,bool*)));

}

void MainWindow::addMarkerMenu(int index,Marker* marker){
    if(objectEditor!=nullptr) objectEditor->deleteLater();

    MarkerMenu* markermenu= new MarkerMenu(this);
    objectEditor = markermenu;
    markermenu->editMarker(index,marker);
    ui->objectEditor->addWidget(markermenu);

    connect(markermenu,SIGNAL(markerChanged(int,Marker*)), modelWindow, SLOT(_markerChanged(int,Marker*)));
}

void MainWindow::addCameraMenu(int index,Camera* camera,bool* runSim){
    if(objectEditor!=nullptr) objectEditor->deleteLater();

    CameraMenu* cameramenu= new CameraMenu(this);
    objectEditor = cameramenu;
    cameramenu->editCamera(index,camera,runSim);
    ui->objectEditor->addWidget(cameramenu);

}


MainWindow::~MainWindow()
{
    delete ui;
}
