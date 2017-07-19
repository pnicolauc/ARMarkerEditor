#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <glsurface/openglwindow.h>
#include <glsurface/modelwindow_gl.h>
#include <ardataloader/ardataloader.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setupGLSignals();
    QUrl selectFile(QString title,QString filters);


private:
    ARDataLoader* ardataloader;
    Ui::MainWindow *ui;
    ModelWindow_GL* modelWindow=nullptr;
    QWidget* objectEditor= nullptr;
    QWidget* glcontainer= nullptr;
public slots:
    void addMarkerMenu(int index,Marker* marker);
    void addCameraMenu(int index,Camera* camera, bool*runSim,CameraParams* camParams);
    bool addGLWindow();
    bool openProject();
    bool opensaveProjectMenu();
    bool saveProject(QString mf,QString vf, QString zf, QString op,QString key);

};

#endif // MAINWINDOW_H
