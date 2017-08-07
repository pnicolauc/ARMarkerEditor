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
#include <menu/saveproject.h>

#include <QOpenGLContext>
#include <glsurface/glsignalemitter.h>
#include <zipreader/zipreader.h>
#include <zipreader/zipwriter.h>
#include <QStandardPaths>

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

    QDir arDir("ardata");
    if(arDir.exists()) arDir.removeRecursively();

    connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui->actionOpen_Model_2,SIGNAL(triggered()),this,SLOT(addGLWindow()));
    connect(ui->actionOpen_Model,SIGNAL(triggered()),this,SLOT(openProject()));
    connect(ui->actionSave_as_Project,SIGNAL(triggered()),this,SLOT(opensaveProjectMenu()));

    this->setFixedSize(970,550);


}

QUrl MainWindow::selectFile(QString title,QString filters){
    QUrl sourceFileName;
    QWidget *container = new QWidget();
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    sourceFileName = dialog.getOpenFileUrl(container, title,QString(""),filters);

    return sourceFileName;
}

bool MainWindow::openProject(){
    QString fileFormat="ARMarker File (*.ardata)";
    QUrl sourceFileName=selectFile("Open a ARMarker file", fileFormat);

    if(sourceFileName.path().length()==0){
        return false;
    }

    QString arFile;

    if(sourceFileName.path().at(0)=="/"){
        arFile=sourceFileName.path().remove(0,1);

    }else arFile=sourceFileName.path();
    ardataloader= new ARDataLoader(arFile);

    QOpenGLContext *context = createOpenGLContext(3, 3);

    QString sourceFn=QString("/ardata/") + ardataloader->modelFolder + "/" + ardataloader->realthreeDFile;
    QString sourceFn2=QString("/ardata/") + ardataloader->modelFolder + "/" + ardataloader->threeDFile;

    qDebug() << sourceFn;
    qDebug() << sourceFn2;

    if (!context)
        qApp->quit();

    if(glcontainer!=nullptr){
        glcontainer->deleteLater();
    }

    QPair<int, int> glVersion = context->format().version();
    bool isOpenGLES = context->format().renderableType() == QSurfaceFormat::OpenGLES;

    qDebug() << "OpenGL Version:" << glVersion << (isOpenGLES ? "ES" : "Desktop");


    //CHANGE PROJECT
    //TODO - add dialog to make sure of replacing gl window
    if (glVersion == qMakePair(3,3) && !isOpenGLES) {
        modelWindow = new ModelWindow_GL(sourceFn, sourceFn2,ModelLoader::RelativePath,ardataloader);//new Scene(OpenGL_Model, ModelLoader::RelativePath);
    }
    modelWindow->initializeWindow(context, 40);

    glcontainer = QWidget::createWindowContainer(modelWindow, this);
    glcontainer->setMinimumSize(500, 500);
    glcontainer->setMaximumSize(1000, 1000);
    glcontainer->setFocusPolicy(Qt::TabFocus);
    ui->glContainer->addWidget(glcontainer);

    setupGLSignals();

}
bool MainWindow::saveProject(QString mf,QString vf, QString zf, QString op,QString key){
    if(glcontainer!=nullptr){
        qDebug() << "Saving...";
        ardataloader= modelWindow->getSaveARData();
        ardataloader->zipfile = zf;
        ardataloader->key = key;
        QFileInfo f1(modelWindow->m_filepath);
        QFileInfo f2(modelWindow->m_filepath2);
        ardataloader->fullModelfolder = f1.absoluteDir().absolutePath();
        ardataloader->fullModelfolder2= f2.absoluteDir().absolutePath();

        if(ardataloader->modelFolder.length()==0)
            ardataloader->modelFolder= QString("model");
        ardataloader->threeDFile =f2.fileName();
        ardataloader->realthreeDFile=f1.fileName();
        ardataloader->save(op);
    }
}

bool MainWindow::opensaveProjectMenu(){
    if(objectEditor!=nullptr) objectEditor->deleteLater();

    SaveProject* savePr= new SaveProject(this);
    objectEditor = savePr;
    ui->objectEditor->addWidget(savePr);

    savePr->setModelFolder(modelWindow->m_filepath);
    savePr->setVirtualFolder(modelWindow->m_filepath2);
    savePr->setSavePath(QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory)+"new.ardata");

    if(ardataloader!=nullptr){
        savePr->setKey(ardataloader->key);
        savePr->xml = ardataloader->xmlfile;
    }
    connect(savePr,SIGNAL(saveProj(QString, QString, QString,QString,QString)), this, SLOT(saveProject(QString, QString, QString,QString,QString)));

}

bool MainWindow::addGLWindow(){
    QString fileFormat="3D Model (*.fbx *.dae *.gltf *.glb";
    fileFormat+=" *.blend *.3ds *.ase *.obj *.ifc *.xgl *.zgl";
    fileFormat+=" *.ply *.dxf *.lwo *.lws *.lxo *.stl *.x";
    fileFormat+=" *.ac *.ms3d *.cob *.scn)";

    QUrl sourceFileName=selectFile("Open Real 3D Model", fileFormat);

    if(sourceFileName.path().length()==0){
        return false;
    }
    QUrl sourceFileName2=selectFile("Open Virtual 3D Model", fileFormat);

    if(sourceFileName2.path().length()==0){
        return false;
    }
    QOpenGLContext *context = createOpenGLContext(3, 3);

    if (!context)
        qApp->quit();

    if(glcontainer!=nullptr){
        glcontainer->deleteLater();
    }

    QPair<int, int> glVersion = context->format().version();
    bool isOpenGLES = context->format().renderableType() == QSurfaceFormat::OpenGLES;

    qDebug() << "OpenGL Version:" << glVersion << (isOpenGLES ? "ES" : "Desktop");

    qDebug() << sourceFileName.path() << " " << sourceFileName2.path();

    //CHANGE PROJECT
    //TODO - add dialog to make sure of replacing gl window
    if (glVersion == qMakePair(3,3) && !isOpenGLES) {
        modelWindow = new ModelWindow_GL(sourceFileName.path(),sourceFileName2.path(), ModelLoader::RelativePath);//new Scene(OpenGL_Model, ModelLoader::RelativePath);
    }
    modelWindow->initializeWindow(context, 40);

    glcontainer = QWidget::createWindowContainer(modelWindow, this);
    glcontainer->setMinimumSize(500, 500);
    glcontainer->setMaximumSize(1000, 1000);
    glcontainer->setFocusPolicy(Qt::TabFocus);
    ui->glContainer->addWidget(glcontainer);

    setupGLSignals();

}


void MainWindow::setupGLSignals(){
    connect(modelWindow->entities.glSignalEmitter,SIGNAL(editMarker(int,Marker*)), this, SLOT(addMarkerMenu(int,Marker*)));
    connect(modelWindow->entities.glSignalEmitter,SIGNAL(editCamera(int,Camera*,bool*,CameraParams*,int*)), this, SLOT(addCameraMenu(int,Camera*,bool*,CameraParams*,int*)));

}

void MainWindow::addMarkerMenu(int index,Marker* marker){
    if(objectEditor!=nullptr) objectEditor->deleteLater();

    MarkerMenu* markermenu= new MarkerMenu(this);
    objectEditor = markermenu;
    markermenu->editMarker(index,marker);
    ui->objectEditor->addWidget(markermenu);

    connect(markermenu,SIGNAL(markerChanged(int,Marker*)), modelWindow, SLOT(_markerChanged(int,Marker*)));
}

void MainWindow::addCameraMenu(int index,Camera* camera,bool* runSim,CameraParams* camParams,int* simCount){
    if(objectEditor!=nullptr) objectEditor->deleteLater();

    CameraMenu* cameramenu= new CameraMenu(this);
    objectEditor = cameramenu;
    cameramenu->editCamera(index,camera,runSim,camParams,simCount);
    ui->objectEditor->addWidget(cameramenu);
}


MainWindow::~MainWindow()
{
    delete ui;
}
