#ifndef ENTITIES_H
#define ENTITIES_H


#include <QOpenGLFunctions_3_3_Core>
#include <QVector>
#include <QCursor>
#include <string>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include "modelloader.h"
#include <glsurface/openglwindow.h>
#include <glsurface/glsignalemitter.h>

enum CreateMode{ CREATE_MARKER,CREATE_CAMERA,NONE };

class Entities
{
public:
    Entities();

    QCursor getCursor(CreateMode cm);

    int createMarker(QVector3D pos,QVector3D rot,float angle);
    int createCamera(QVector3D pos,QVector3D rot,QVector2D scale,float angle);
    Marker getMarker(int index);
    Marker* getMarkerPtr(int index);
    Camera getCamera(int index);

    QOpenGLTexture* getCameraTexture();

    int cameraCount();
    int markerCount();

    void emitChangedMarkerSignal(int index);

    QMatrix4x4 getCubeMapProjectionMatrix();
    GLSignalEmitter* glSignalEmitter;


    bool runSim;
    int simIndex;
    int selectedCam;
    CameraParams camParams;


    QVector<Camera> cameras;
    QVector<Marker> markers;

private:
    QCursor defaultCursor;
    QCursor markerCursor;
    QCursor cameraCursor;

    int selectedMarker;


    QOpenGLTexture* defaultMarkerTexture;
    QImage* defaultMarkerImage;

    QOpenGLTexture* defaultCameraTexture;
    QImage* defaultCameraImage;


    CreateMode createMode;
    bool create_mode;

    QSharedPointer<Node> m_cameraNode;
    QSharedPointer<Node> m_markerNode;

    QMatrix4x4 m_projection_cubemap;
};

#endif // ENTITIES_H
