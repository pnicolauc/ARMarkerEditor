#ifndef THREEDDATA_H
#define THREEDDATA_H


#include <string>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLFunctions>
#include "modelloader.h"
class ThreeDData
{
public:
    ThreeDData();
private:
    QOpenGLVertexArrayObject m_vao;

    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLBuffer m_textureUVBuffer;
    QOpenGLBuffer m_indexBuffer;

    QSharedPointer<Node> m_cameraNode;
    QSharedPointer<Node> m_markerNode;
    QSharedPointer<Node> m_rootNode;

    QMatrix4x4 m_model;

};

#endif // THREEDDATA_H
