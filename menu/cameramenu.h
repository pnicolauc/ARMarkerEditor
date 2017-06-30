#ifndef CAMERAMENU_H
#define CAMERAMENU_H

#include <QWidget>
#include <glsurface/openglwindow.h>
namespace Ui {
class CameraMenu;
}

class CameraMenu : public QWidget
{
    Q_OBJECT

public:
    explicit CameraMenu(QWidget *parent = 0);
    ~CameraMenu();
    void editCamera(int index,Camera* camera,bool* runSim,CameraParams* camParams);


private:
    Ui::CameraMenu *ui;
    int m_Index;
    Camera *m_Camera;
    CameraParams *m_CameraParams;
    bool* m_runSim;
public slots:
    void runSimulation();
    void changeCameraParams();
    void changeCamera();

};

#endif // CAMERAMENU_H
