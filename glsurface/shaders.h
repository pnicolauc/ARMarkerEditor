#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLShaderProgram>

enum Pass { Normals, Depth , MarkerTex , Picking, Full , CameraSim};

class Shader
{
public:
    Shader();
    void createShaderProgram(QString vShader, QString fShader,QOpenGLShaderProgram* shaderProgram);
    void setPass(Pass curr){pass=curr;}
    void setupShader(QVector4D lightPos,QVector3D lightInt);
private:
    Pass pass;
    QOpenGLShaderProgram m_shaderProgram;
    QOpenGLShaderProgram m_NormalshaderProgram;
    QOpenGLShaderProgram m_DepthshaderProgram;
    QOpenGLShaderProgram m_MarkerTextureProgram;
    QOpenGLShaderProgram m_ObjectPicking;
    QOpenGLShaderProgram m_ShadowMapProgram;
};

#endif // SHADER_H
