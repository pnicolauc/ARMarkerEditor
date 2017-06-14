#ifndef MODELWINDOW_GL_3_3_H
#define MODELWINDOW_GL_3_3_H

#include <string>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLFunctions>
#include "modelloader.h"
#include "openglwindow.h"
#include <QOpenGLFramebufferObject>
#include <QVector>
#include <glsurface/glsignalemitter.h>

enum Pass { Normals, Depth , MarkerTex , Picking, Full , CameraSim};
enum CreateMode{ CREATE_MARKER,CREATE_CAMERA,NONE };
class ModelWindow_GL : public QOpenGLFunctions_3_3_Core, public OpenGLWindow
{

public:
    ModelWindow_GL(QString filepath, ModelLoader::PathType pathType, QString texturePath="");
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent * ev);
    void mouseMoveEvent(QMouseEvent* event);
    void drawCameras();
    void drawMarkers();
    void setupTextures();
    void createMarker(QVector3D pos,QVector3D rot,float angle);
    void createEntity();
    void createCamera(QVector3D pos,QVector3D rot,float angle);

    GLSignalEmitter* glSignalEmitter;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void cleanupGL();
    void markerChanged(int index,Marker* marker);

signals:
    Marker* markerCreated(Marker* marker);
    Camera* cameraCreated(Camera* camera);
private:
    void createShaderProgram( QString vShader, QString fShader,QOpenGLShaderProgram* shaderProgram);
    void createBuffers();
    void createAttributes();
    void setupLightingAndMatrices();
    void RenderPass(Pass currPass,bool renderFBO,bool renderModel,bool renderEntities);
    void createAttributes(QOpenGLShaderProgram* shaderprogram);
    void depth();

    void draw();
    void setupCamera(QVector3D eye,QVector3D center,QVector3D up);
    void drawNode(const Node *node, QMatrix4x4 objectMatrix);
    void setMaterialUniforms(MaterialInfo &mater);
    void setShaderUniformNodeValues(QMatrix4x4 objectMatrix);
    void setupShader();
    void setupRenderTarget();
    void releaseRenderTarget();

     #define M_PI_2 1.57079632679489661923
    int height, width;

    float xAngle,yAngle;


    GLfloat *depthTex;
    GLfloat *normsTex;

    int selectedMarker;


    GLuint text;
    QOpenGLTexture* defaultMarkerTexture;
    QImage* defaultMarkerImage;

    QImage fboPickingImage;
    float lastMouseWorldPos[4];
    float lastMouseWorldNormals[4];

    QVector<Camera> cameras;
    QVector<Marker> markers;


    int lastMouseX,lastMouseY;
    bool mouseDrag;

    int mouseSelectedX,mouseSelectedY;
    bool mouseDClick;
    CreateMode createMode;


    QVector3D cameraPos;
    QVector3D cameraVec;
    QVector3D cameraForward;
    QVector3D upVec;

    QOpenGLShaderProgram m_shaderProgram;
    QOpenGLShaderProgram m_NormalshaderProgram;
    QOpenGLShaderProgram m_DepthshaderProgram;
    QOpenGLShaderProgram m_MarkerTextureProgram;
    QOpenGLShaderProgram m_ObjectPicking;
    QOpenGLShaderProgram m_ShadowMapProgram;

    Pass pass;

    QOpenGLVertexArrayObject m_vao;

    QOpenGLFramebufferObject *int_fbo=nullptr;
    QOpenGLFramebufferObject *float_fbo=nullptr;

    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLBuffer m_textureUVBuffer;
    QOpenGLBuffer m_indexBuffer;

    QSharedPointer<Node> m_cameraNode;
    QSharedPointer<Node> m_markerNode;
    QSharedPointer<Node> m_rootNode;

    QMatrix4x4 m_projection, m_view, m_model, m_shadow;
    QOpenGLTexture* shadowTexture;

    QString m_filepath;
    ModelLoader::PathType m_pathType;
    QString m_texturePath;

    LightInfo m_lightInfo;
    MaterialInfo m_materialInfo;

    bool m_error;
};

#endif // MODELWINDOW_GL_3_3_H
