#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <glsurface/openglwindow.h>
#include <glsurface/modelwindow_gl.h>

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

private:

    Ui::MainWindow *ui;
    ModelWindow_GL* modelWindow=nullptr;
    QWidget* objectEditor= nullptr;

public slots:
    void addMarkerMenu(int index,Marker* marker);
    void addCameraMenu(int index,Camera* camera, bool*runSim);
    void addGLWindow();

};

#endif // MAINWINDOW_H
