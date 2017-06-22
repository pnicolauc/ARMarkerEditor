#ifndef VIEWCAMERA_H
#define VIEWCAMERA_H

#include <QOpenGLFunctions_3_3_Core>
#include <QVector3D>
#include <QMatrix4x4>

class ViewCamera
{
public:
    ViewCamera();

    void walkForward();
    void walkBackward();
    void mouseRotate(float valx,float valy);
    QMatrix4x4 getViewM();
    QMatrix4x4 getProjM();
    void resize(int h,int w);
    void setupCamera(QVector3D eye=QVector3D(0,0,0),QVector3D center=QVector3D(0,0,0),QVector3D up=QVector3D(0,0,0));
    QVector3D getForwardPos();
    void eagleView();

    bool shiftPressed;

private:
    QVector3D rotate(QVector3D Vx,QVector3D axis, float angle);


    QVector3D cameraPos;
    QVector3D cameraVec;
    QVector3D cameraForward;
    QVector3D upVec;

    QMatrix4x4 m_view;

    int height,width;

};

#endif // VIEWCAMERA_H
