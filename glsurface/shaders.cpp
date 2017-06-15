#include "shaders.h"

Shaders::Shaders()
{

}

void Shaders::bindRenderTargets(Pass pass){

}


void Shaders::bindProgram(QOpenGLShaderProgram* shaderprogram,
                          Pass pass,LightInfo m_lightInfo){

    shaderprogram->bind();
    shaderprogram->setUniformValue( "lightPosition", m_lightInfo.Position );
    shaderprogram->setUniformValue( "lightIntensity", m_lightInfo.Intensity );

    if(pass==MarkerTex)
            shaderprogram->setUniformValue("texture", 0);
}


/*
QOpenGLShaderProgram* Shaders::getShaderProgram(Pass pass){
    switch(pass){
        case Normals:
            return m_NormalshaderProgram;
            break;
        case Depth:
            return m_DepthshaderProgram;
            break;
        case Picking:
            return m_ObjectPicking;
            break;
        case MarkerTex:
            return m_MarkerTextureProgram;
            break;
        case CameraSim:
            return m_ShadowMapProgram;
            break;
        case Full:
            return m_shaderProgram;
            break;
    }
    return m_shaderProgram;

}

void Shaders::setShaderUniformNodeValues(Pass pass,
                                         QMatrix4x4 objectMatrix,
                                         QMatrix4x4 m_model,
                                         QMatrix4x4 m_view,
                                         QMatrix4x4 m_projection,
                                         bool simOrCrMode,
                                         QMatrix4x4 shadow,
                                         GLint text
                                         ){
    QMatrix4x4 modelMatrix = m_model * objectMatrix;
    QMatrix4x4 modelViewMatrix = m_view * modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_projection * modelViewMatrix;
    switch (pass) {
    case Normals:
        m_NormalshaderProgram->setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_NormalshaderProgram->setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_NormalshaderProgram->setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_NormalshaderProgram->setUniformValue( "P", m_projection );
        m_NormalshaderProgram->setUniformValue( "V", m_view );
        break;
    case Depth:
        m_DepthshaderProgram->setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_DepthshaderProgram->setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_DepthshaderProgram->setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_DepthshaderProgram->setUniformValue( "P", m_projection );
        m_DepthshaderProgram->setUniformValue( "V", m_view );
        break;
    case Picking:
        m_ObjectPicking->setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_ObjectPicking->setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_ObjectPicking->setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_ObjectPicking->setUniformValue( "P", m_projection );
        m_ObjectPicking->setUniformValue( "V",m_view );
        break;
    case MarkerTex:
        m_MarkerTextureProgram->setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_MarkerTextureProgram->setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_MarkerTextureProgram->setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_MarkerTextureProgram->setUniformValue( "P", m_projection );
        m_MarkerTextureProgram->setUniformValue( "V", m_view );
        m_MarkerTextureProgram->setUniformValue( "create_mode", simOrCrMode );

        break;
    case Full:{
        QMatrix4x4 shadowMVP = m_projection * (shadow *modelMatrix);
        m_shaderProgram->setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_shaderProgram->setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_shaderProgram->setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_shaderProgram->setUniformValue( "P", m_projection );
        m_shaderProgram->setUniformValue( "V", m_view );
        if(simOrCrMode)
            m_shaderProgram->setUniformValue("shadow",shadowMVP );

        m_shaderProgram->setUniformValue( "cameraSim", simOrCrMode );
        glBindTexture(GL_TEXTURE_2D, text);
        m_shaderProgram->setUniformValue("texture",0);


        break;}
    case CameraSim:{
        m_ShadowMapProgram->setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_ShadowMapProgram->setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_ShadowMapProgram->setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_ShadowMapProgram->setUniformValue( "P", m_projection );
        m_ShadowMapProgram->setUniformValue( "V", m_view );
        break;}
    default:
        break;
    }
}
*/
void Shaders::setMaterialUniforms(QOpenGLShaderProgram* program,MaterialInfo &mater)
{
    program->setUniformValue( "Ka", mater.Ambient );
    program->setUniformValue( "Kd", mater.Diffuse );
    program->setUniformValue( "Ks", mater.Specular );
    program->setUniformValue( "shininess", mater.Shininess );
}

