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

    defaultCameraImage=new QImage(QString(":/resources/textures/walkable.jpg"));
    defaultCameraTexture= new QOpenGLTexture(defaultCameraImage->mirrored());

    m_projection_cubemap.setToIdentity();
    m_projection_cubemap.perspective(
                90.0f,          // field of vision
                1.0f,         // aspect ratio
                0.5f,           // near clipping plane
                100.0f);       // far clipping plane

    runSim=false;
    camParams.height=1.5;
    camParams.focalLength=15.0;
    camParams.horizontalAOV=60.0;
    camParams.verticalAOV=60.0;

    camParams.projection.setToIdentity();
    camParams.projection.perspective(camParams.verticalAOV, camParams.horizontalAOV/camParams.verticalAOV, .3f, 100);
}

QMatrix4x4 Entities::getCubeMapProjectionMatrix(){
    return m_projection_cubemap;
}

QOpenGLTexture* Entities::getCameraTexture(){
 return defaultCameraTexture;
}

int Entities::createMarker(QString name,QVector2D sc,QVector3D pos,QVector3D rot,float angle){
    Marker marker;

    marker.name= name;
    marker.position= pos;
    marker.rotation= rot;
    marker.angle=angle;
    marker.scale=sc;
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


int Entities::createCamera(QVector3D pos,QVector3D rot,QVector2D scale,float angle,int* simCount){
    Camera camera;
    camera.position= pos;
    camera.rotation= rot;
    camera.angle=angle;
    camera.scale=scale;
    cameras.push_back(camera);

    selectedCam=cameras.size()-1;

    emit glSignalEmitter->editCamera(cameras.size()-1,&cameras[cameras.size()-1],&runSim,&camParams,simCount);
    return  ((-cameras.size()));
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
