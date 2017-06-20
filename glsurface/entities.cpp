#include "entities.h"

Entities::Entities()
{
    QPixmap markerCursorPic=QPixmap(":/resources/icons/marker.png").scaled(20,20);
    QPixmap cameraCursorPic=QPixmap(":/resources/icons/camera.png").scaled(20,20);

    markerCursor = QCursor(markerCursorPic);
    cameraCursor = QCursor(cameraCursorPic);
    defaultCursor= QCursor(Qt::ArrowCursor);

    createMode= NONE;
    glSignalEmitter = new GLSignalEmitter();

    defaultMarkerImage=new QImage(QString(":/resources/textures/marker.jpg"));
    defaultMarkerTexture= new QOpenGLTexture(defaultMarkerImage->mirrored());

    m_projection_cubemap.setToIdentity();
    m_projection_cubemap.perspective(
                90.0f,          // field of vision
                1.0f,         // aspect ratio
                0.5f,           // near clipping plane
                100.0f);       // far clipping plane

    runSim=false;
}

QMatrix4x4 Entities::getCubeMapProjectionMatrix(){
    return m_projection_cubemap;
}

int Entities::createMarker(QVector3D pos,QVector3D rot,float angle){
    Marker marker;
    marker.position= pos;
    marker.rotation= rot;
    marker.angle=angle;
    marker.scale=QVector2D(1.0,1.0);
    marker.texture = defaultMarkerTexture;
    marker.image = defaultMarkerImage;

    markers.push_back(marker);
    selectedMarker=markers.size()-1;

    emit glSignalEmitter->editMarker(markers.size()-1,&markers[markers.size()-1]);

    return markers.size()-1;
}

void Entities::emitChangedMarkerSignal(int index){
    emit glSignalEmitter->editMarker(index,&markers[index]);
}


int Entities::markerCount(){
    return markers.size();
}

int Entities::cameraCount(){
    return cameras.size();
}
Marker Entities::getMarker(int index){
    return markers[index];
}


Marker* Entities::getMarkerPtr(int index){
    return &markers[index];
}

Camera Entities::getCamera(int index){
    return cameras[index];
}


void Entities::createCamera(QVector3D pos,QVector3D rot,float angle){
    Camera camera;
    camera.position= pos;
    camera.rotation= rot;
    camera.angle=angle;
    cameras.push_back(camera);

    selectedCam=cameras.size()-1;

    emit glSignalEmitter->editCamera(cameras.size()-1,&cameras[cameras.size()-1],&runSim);
}

QCursor Entities::getCursor(CreateMode cm){
    switch (cm) {
    case CREATE_CAMERA:
        return cameraCursor;
        break;
    case CREATE_MARKER:
        return markerCursor;
        break;
    case NONE:
        return defaultCursor;
        break;
    default:
        break;
    }

    return defaultCursor;
}
