#ifndef SHADERS_H
#define SHADERS_H

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
#include <QMatrix4x4>


class Shaders
{
public:
    Shaders();
    void createShaders(QOpenGLVertexArrayObject* vao,
                       QOpenGLBuffer* m_vertexBuffer,
                       QOpenGLBuffer* m_normalBuffer,
                       QOpenGLBuffer* m_textureUVBuffer);
    void bindProgram(QOpenGLShaderProgram* shaderprogram,
                     Pass pass,LightInfo m_lightInfo);

    void setShaderUniformNodeValues(Pass pass,
                                    QMatrix4x4 objectMatrix,
                                    QMatrix4x4 m_model,
                                    QMatrix4x4 m_view,
                                    QMatrix4x4 m_projection,
                                    bool simOrCrMode = false,
                                    QMatrix4x4 shadow = QMatrix4x4(),
                                    GLint text=0);
   void setMaterialUniforms(QOpenGLShaderProgram* program,MaterialInfo &mater);
   QOpenGLShaderProgram* getShaderProgram(Pass pass);

    void bindRenderTargets(Pass pass);

};

#endif // SHADERS_H
