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
#include <glsurface/entities.h>
#include <glsurface/viewcamera.h>
#include <glsurface/screen.h>

#include <glsurface/shaders.h>
#include <ardataloader/ardataloader.h>

class ModelWindow_GL : public QOpenGLFunctions_3_3_Core, public OpenGLWindow
{

public:
    ModelWindow_GL(QString filepath,QString filepath2, ModelLoader::PathType pathType,ARDataLoader* dataLoader= new ARDataLoader());
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent * ev);
    void keyReleaseEvent(QKeyEvent *event);

    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent *event);

    void drawCameras();
    void drawMarkers();
    void createEntity();
    ARDataLoader* getSaveARData();
    Entities entities;
    ARDataLoader* ardataloader;

    QString m_filepath,m_filepath2;

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
    QOpenGLShaderProgram* getCurrentProgram();

    void draw();
    void drawNode(const Node *node, QMatrix4x4 objectMatrix);
    void setMaterialUniforms(MaterialInfo &mater);
    void setShaderUniformNodeValues(QMatrix4x4 objectMatrix);

    void setupRenderTarget();
    void releaseRenderTarget();

    ViewCamera viewCam;
    Shaders shaders;
    Screen screen;


    #define M_PI_2 1.57079632679489661923

    GLfloat *depthTex;
    GLfloat *normsTex;

    int selectedEntity; //negative for cameras, positive for markers
    QImage fboPickingImage;
    float lastMouseWorldPos[4];
    float lastMouseWorldNormals[4];


    int rectCamera;

    int rectTopLeftX,rectTopLeftY;
    bool rectTopLeft;
    float rectWorldPos0[4];
    float rectWorldPos1[4];
    float rectWorldPos3[4];
    float rectWorldPos4[4];

    QImage imagemd0;
    QImage imagemd1;
    QImage imagemd2;
    QImage imagemd3;
    QImage imagemd4;
    QImage imagemd5;

    QOpenGLTexture* cubeMD;

    GLuint md0;
    GLuint md1;
    GLuint md2;
    GLuint md3;
    GLuint md4;
    GLuint md5;

    GLuint sim0;
    GLuint sim1;
    GLuint sim2;
    GLuint sim3;
    GLuint sim4;
    GLuint sim5;
    bool cameraSim;

    bool usePreviousCameraSimPos;

    int simCount;
    int stepx;
    int stepy;

    QVector<QVector3D> simDirs;

    bool fpsView;

    CreateMode createMode;
    bool create_mode;


    float scroll;

    QOpenGLShaderProgram* curr_Program;

    QOpenGLShaderProgram m_shaderProgram;
    QOpenGLShaderProgram m_NormalshaderProgram;
    QOpenGLShaderProgram m_DepthshaderProgram;
    QOpenGLShaderProgram m_MarkerTextureProgram;
    QOpenGLShaderProgram m_ObjectPicking;
    QOpenGLShaderProgram m_ShadowMapProgram;
    QOpenGLShaderProgram m_MarkerDepthProgram;

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
    QSharedPointer<Node> m_virtualNode;

    QMatrix4x4 m_model, m_SimView0,
    m_SimView1,m_SimView2,
    m_SimView3,m_SimView4,
    m_SimView5;

    QMatrix4x4  m_prevSimView0,
    m_prevSimView1,m_prevSimView2,
    m_prevSimView3,m_prevSimView4,
    m_prevSimView5;

    QOpenGLTexture* shadowTexture;

    ModelLoader::PathType m_pathType;

    DRAW_MODEL draw_Model;
    QString m_texturePath;

    LightInfo m_lightInfo;
    MaterialInfo m_materialInfo;

    bool m_error;
};

#endif // MODELWINDOW_GL_3_3_H
