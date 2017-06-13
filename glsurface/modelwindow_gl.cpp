#include "modelwindow_gl.h"
#include <QOpenGLFramebufferObject>
#include <QVector>

ModelWindow_GL::ModelWindow_GL(QString filepath, ModelLoader::PathType pathType, QString texturePath) :
    OpenGLWindow()
  , m_indexBuffer(QOpenGLBuffer::IndexBuffer)
  , m_filepath(filepath)
  , m_pathType(pathType)
  , m_texturePath(texturePath)
  , m_error(false)
{
    glSignalEmitter = new GLSignalEmitter();
}

void ModelWindow_GL::setupTextures(){
    defaultMarkerImage=new QImage(QString(":/resources/textures/marker.jpg"));
    defaultMarkerTexture= new QOpenGLTexture(defaultMarkerImage->mirrored());
}

void ModelWindow_GL::initializeGL()
{
    mouseDrag=false;
    mouseDClick=false;
    createMode= NONE;
    this->initializeOpenGLFunctions();

    createShaderProgram(":/resources/shaders/ads_fragment.vert", ":/resources/shaders/ads_fragment.frag",&m_shaderProgram);
    createShaderProgram(":/resources/shaders/normals.vert", ":/resources/shaders/normals.frag",&m_NormalshaderProgram);
    createShaderProgram(":/resources/shaders/depth.vert", ":/resources/shaders/depth.frag",&m_DepthshaderProgram);
    createShaderProgram(":/resources/shaders/markertexture.vert", ":/resources/shaders/markertexture.frag",&m_MarkerTextureProgram);
    createShaderProgram(":/resources/shaders/picking.vert", ":/resources/shaders/picking.frag",&m_ObjectPicking);
    createShaderProgram(":/resources/shaders/shadow.vert", ":/resources/shaders/shadow.frag",&m_ShadowMapProgram);


    createBuffers();
    createAttributes(&m_shaderProgram);
    createAttributes(&m_NormalshaderProgram);
    createAttributes(&m_DepthshaderProgram);
    createAttributes(&m_MarkerTextureProgram);
    createAttributes(&m_ObjectPicking);
    createAttributes(&m_ShadowMapProgram);

    setupLightingAndMatrices();

    setupTextures();

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f ,1.0f);
}

void ModelWindow_GL::createShaderProgram(QString vShader, QString fShader,QOpenGLShaderProgram* shaderProgram)
{
    // Compile vertex shader
    if ( !m_error && !shaderProgram->addShaderFromSourceFile( QOpenGLShader::Vertex, vShader.toUtf8() ) ) {
        qDebug() << "Error in vertex shader:" << shaderProgram->log();
        m_error = true;
    }

    // Compile fragment shader
    if ( !m_error && !shaderProgram->addShaderFromSourceFile( QOpenGLShader::Fragment, fShader.toUtf8() ) ) {
        qDebug() << "Error in fragment shader:" << shaderProgram->log();
        m_error = true;
    }

    // Link the shaders together into a program
    if ( !m_error && !shaderProgram->link() ) {
        qDebug() << "Error linking shader program:" << shaderProgram->log();
        m_error = true;
    }
}

void ModelWindow_GL::createBuffers()
{
    ModelLoader model;

    if(!model.Load(m_filepath, m_pathType))
    {
        m_error = true;
        return;
    }
    QVector<float> *vertices;
    QVector<float> *normals;
    QVector<QVector<float> > *textureUV;
    QVector<unsigned int> *indices;

    model.getBufferData(&vertices, &normals, &indices);
    model.getTextureData(&textureUV, 0, 0);

    // Create a vertex array object
    m_vao.create();
    m_vao.bind();

    // Create a buffer and copy the vertex data to it
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate( &(*vertices)[0], vertices->size() * sizeof( float ) );

    // Create a buffer and copy the vertex data to it
    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalBuffer.bind();
    m_normalBuffer.allocate( &(*normals)[0], normals->size() * sizeof( float ) );

    if(textureUV != 0 && textureUV->size() != 0)
    {
        // Create a buffer and copy the vertex data to it
        m_textureUVBuffer.create();
        m_textureUVBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_textureUVBuffer.bind();
        int texSize = 0;
        for(int ii=0; ii<textureUV->size(); ++ii)
            texSize += textureUV->at(ii).size();

        m_textureUVBuffer.allocate( &(*textureUV)[0][0], texSize * sizeof( float ) );
    }

    // Create a buffer and copy the index data to it
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( &(*indices)[0], indices->size() * sizeof( unsigned int ) );

    m_rootNode = model.getNodeData();
    m_cameraNode = model.getCameraData();
    m_markerNode= model.getMarkerData();

}

void ModelWindow_GL::createAttributes(QOpenGLShaderProgram* shaderprogram)
{
    if(m_error)
        return;

    m_vao.bind();
    // Set up the vertex array state
    shaderprogram->bind();

    // Map vertex data to the vertex shader's layout location '0'
    m_vertexBuffer.bind();
    shaderprogram->enableAttributeArray( 0 );      // layout location
    shaderprogram->setAttributeBuffer( 0,          // layout location
                                        GL_FLOAT,   // data's type
                                        0,          // Offset to data in buffer
                                        3);         // number of components (3 for x,y,z)

    // Map normal data to the vertex shader's layout location '1'
    m_normalBuffer.bind();
    shaderprogram->enableAttributeArray( 1 );      // layout location
    shaderprogram->setAttributeBuffer( 1,          // layout location
                                        GL_FLOAT,   // data's type
                                        0,          // Offset to data in buffer
                                        3);         // number of components (3 for x,y,z)

    if(m_textureUVBuffer.isCreated()) {
        m_textureUVBuffer.bind();
        shaderprogram->enableAttributeArray( 2 );      // layout location
        shaderprogram->setAttributeBuffer( 2,          // layout location
                                            GL_FLOAT,   // data's type
                                            0,          // Offset to data in buffer
                                            2);         // number of components (2 for u,v)
    }


}
QVector3D rotate(QVector3D Vx,QVector3D axis, float angle) {
  float ca = cos(angle);
  float sa = sin(angle);

  QVector3D cross = QVector3D::crossProduct(Vx,axis);

  float dot= QVector3D::dotProduct(axis,Vx);

  QVector3D r=Vx*ca + cross*sa + dot*axis*(1-ca);

  return r;
}
void ModelWindow_GL::setupLightingAndMatrices()
{
    m_view.setToIdentity();

    cameraPos = QVector3D(0.0f, 0.0f, 1.0f);
    cameraVec = QVector3D(0.0f, 0.0f, 0.0f);
    upVec = QVector3D(0.0f, 1.0f, 0.0f);

    cameraForward = (QVector3D(cameraVec - cameraPos));
    cameraForward.normalize();
    m_view.lookAt(
                cameraPos,    // Camera Position
                cameraVec,    // Point camera looks towards
                upVec);   // Up vector
    float aspect = 4.0f/3.0f;
    m_projection.setToIdentity();
    m_projection.perspective(
                60.0f,          // field of vision
                aspect,         // aspect ratio
                0.3f,           // near clipping plane
                1000.0f);       // far clipping plane

    m_lightInfo.Position = QVector4D( -1.0f, 1.0f, 1.0f, 1.0f );
    m_lightInfo.Intensity = QVector3D( 1.0f, 1.0f, 1.0f);

    m_materialInfo.Ambient = QVector3D( 0.05f, 0.2f, 0.05f );
    m_materialInfo.Diffuse = QVector3D( 0.3f, 0.5f, 0.3f );
    m_materialInfo.Specular = QVector3D( 0.6f, 0.6f, 0.6f );
    m_materialInfo.Shininess = 50.0f;
}

void ModelWindow_GL::resizeGL(int w, int h)
{
    height=h;
    width=w;
    glViewport( 0, 0, w, h );
    normsTex = new GLfloat[4*width*height];
    depthTex = new GLfloat[4*width*height];
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, (float)w/h, .3f, 1000);
}

void ModelWindow_GL::drawCameras(){
    for(int i=0;i< cameras.size();i++){
        Camera cam= cameras.at(i);
        m_model.setToIdentity();
        m_model.translate(cam.position);
        m_model.rotate(cam.angle,cam.rotation);
        m_vao.bind();
        drawNode(m_cameraNode.data(), QMatrix4x4());
        m_vao.release();
    }
    m_model.setToIdentity();
}

void ModelWindow_GL::drawMarkers(){
    if(pass == Full){
        pass=MarkerTex;
        m_MarkerTextureProgram.bind();
        m_MarkerTextureProgram.setUniformValue( "lightPosition", m_lightInfo.Position );
        m_MarkerTextureProgram.setUniformValue( "lightIntensity", m_lightInfo.Intensity );
        m_MarkerTextureProgram.setUniformValue("texture", 0);
    }
    for(int i=0;i< markers.size();i++){
        Marker mk= markers.at(i);
        mk.texture->bind();

        if(pass==MarkerTex){
            if(selectedMarker==i)
                m_MarkerTextureProgram.setUniformValue( "selected", true );
            else
                m_MarkerTextureProgram.setUniformValue( "selected", false );
        }
        if(pass == Picking)
            m_ObjectPicking.setUniformValue( "id", (float)(i/255.0) );

        m_model.setToIdentity();
        m_model.translate(mk.position);
        m_model.rotate(mk.angle,mk.rotation);
        m_model.scale(mk.scale.x(),mk.scale.y(),0.0);

        m_vao.bind();
        drawNode(m_markerNode.data(), QMatrix4x4());
        m_vao.release();
    }
    m_model.setToIdentity();
}

void ModelWindow_GL::setupShader(){
    switch(pass){
        case Normals:
            m_NormalshaderProgram.bind();
            m_NormalshaderProgram.setUniformValue( "lightPosition", m_lightInfo.Position );
            m_NormalshaderProgram.setUniformValue( "lightIntensity", m_lightInfo.Intensity );
            break;
        case Depth:
            m_DepthshaderProgram.bind();
            m_DepthshaderProgram.setUniformValue( "lightPosition", m_lightInfo.Position );
            m_DepthshaderProgram.setUniformValue( "lightIntensity", m_lightInfo.Intensity );
            break;
        case Picking:
            m_ObjectPicking.bind();
            m_ObjectPicking.setUniformValue( "lightPosition", m_lightInfo.Position );
            m_ObjectPicking.setUniformValue( "lightIntensity", m_lightInfo.Intensity );
            break;
        case MarkerTex:
            m_MarkerTextureProgram.bind();
            m_MarkerTextureProgram.setUniformValue( "lightPosition", m_lightInfo.Position );
            m_MarkerTextureProgram.setUniformValue( "lightIntensity", m_lightInfo.Intensity );
            break;
        case CameraSim:
            m_ShadowMapProgram.bind();
            m_ShadowMapProgram.setUniformValue( "lightPosition", m_lightInfo.Position );
            m_ShadowMapProgram.setUniformValue( "lightIntensity", m_lightInfo.Intensity );
            break;
        case Full:
            m_shaderProgram.bind();
            m_shaderProgram.setUniformValue( "lightPosition", m_lightInfo.Position );
            m_shaderProgram.setUniformValue( "lightIntensity", m_lightInfo.Intensity );

            break;
    }
}

void ModelWindow_GL::setupRenderTarget(){
    switch(pass){
        case Depth:
        case Normals:
        case CameraSim:
        if(float_fbo==nullptr){
                        QOpenGLFramebufferObjectFormat format2;
                        float_fbo = new QOpenGLFramebufferObject(width,height, QOpenGLFramebufferObject::CombinedDepthStencil,
                                                                 GL_TEXTURE_2D,
                                                                 GL_RGBA32F);
        }
        float_fbo->bind();
        break;
        case Picking:
        if(int_fbo==nullptr){
                        QOpenGLFramebufferObjectFormat format;
                        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
                        int_fbo = new QOpenGLFramebufferObject(width,height, format);
        }
        int_fbo->bind();
        break;
    }
}
void ModelWindow_GL::releaseRenderTarget(){
    switch (pass) {
        case Picking:
            fboPickingImage = int_fbo->toImage();
            break;
        case Depth:
            if(mouseDClick){
                glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
                glReadPixels(mouseSelectedX, height-mouseSelectedY, 1, 1, GL_RGBA, GL_FLOAT, lastMouseWorldPos);
                qDebug() << "Position" << " " << lastMouseWorldPos[2]<< " " <<lastMouseWorldPos[0] << " " << lastMouseWorldPos[1] << " " << lastMouseWorldPos[2];
            }
            break;
        case Normals:
            if(mouseDClick){
                glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
                glReadPixels(mouseSelectedX, height-mouseSelectedY, 1, 1, GL_RGBA, GL_FLOAT, lastMouseWorldNormals);
                qDebug() << "Normals:" <<lastMouseWorldNormals[0] << " " << lastMouseWorldNormals[1] << " " << lastMouseWorldNormals[2];
            }
            break;
        case CameraSim:
            QImage im = float_fbo->toImage();
            shadowTexture = new QOpenGLTexture(im.mirrored());
            im.save("shadow.png");
            break;
    }
}

void ModelWindow_GL::RenderPass(Pass currPass,bool renderFBO,bool renderModel,bool renderEntities ){
    if(m_error)
        return;

    pass=currPass;
    // Clear color and depth buffers
    // Bind shader programs and rendertargets
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    setupShader();
    if(renderFBO)
        setupRenderTarget();
    else{
        int_fbo->bindDefault();
        float_fbo->bindDefault();
    }

    // Bind VAO and draw
    if(renderModel){
        m_vao.bind();
        drawNode(m_rootNode.data(), QMatrix4x4());
        m_vao.release();
    }
    if(renderEntities){
        m_vao.bind();
        drawCameras();
        drawMarkers();
        m_vao.release();
    }

    //release renderTargets
    if(renderFBO)
        releaseRenderTarget();
}

void ModelWindow_GL::mousePressEvent(QMouseEvent* event)
{
    mouseDrag=true;
}
void ModelWindow_GL::mouseReleaseEvent(QMouseEvent* event){
    mouseDrag= false;

}
void ModelWindow_GL::mouseMoveEvent(QMouseEvent* event){
    if(mouseDrag)
    {
        float difX = (lastMouseX - event->x())/100.0;
        float difY = (lastMouseY - event->y())/100.0;

        //Left/Right Camera Rotation
        cameraForward = rotate(cameraForward,upVec, -difX);

        //Up/Down Camera Rotation
        QVector3D upV= QVector3D::crossProduct(upVec,cameraForward);
        rotate(upVec, upV, difY);
        cameraForward =rotate(cameraForward, upV, difY);

    }
    lastMouseX=event->x();
    lastMouseY=event->y();

}

void ModelWindow_GL::mouseDoubleClickEvent(QMouseEvent* event)
{
    QColor color = QColor(fboPickingImage.pixel(event->x(),event->y()));
    if(color.red()!=255) {
        qDebug() << "Clicked on Marker";
        selectedMarker = color.red();
        emit glSignalEmitter->editMarker(selectedMarker,&markers[selectedMarker]);
    }else{
        mouseSelectedX=event->x();
        mouseSelectedY=event->y();
        mouseDClick=true;
    }
}



void ModelWindow_GL::keyPressEvent(QKeyEvent * ev) {
    switch (ev->key()) {
    case  Qt::Key_W:
        cameraPos+=cameraForward/10.0;
        break;
    case Qt::Key_S:
        cameraPos-=cameraForward/10.0;
        break;
    case Qt::Key_C:{
        createMode=CREATE_CAMERA;
        qDebug() << "CREATE_CAMERA";

        break;
        }
    case Qt::Key_M:{
        createMode=CREATE_MARKER;
        qDebug() << "CREATE_MARKER";

        break;
        }
    default:
        break;
    }


}

void ModelWindow_GL::markerChanged(int index,Marker* marker){
   // markers[index]=*markerr;
}

void ModelWindow_GL::setupCamera(QVector3D eye,QVector3D center,QVector3D up){
    m_view.setToIdentity();
    m_view.lookAt(
                eye,    // Camera Position
                center,    // Point camera looks towards
                up);   // Up vector
}
void ModelWindow_GL::createCamera(QVector3D pos,QVector3D rot,float angle){
    Camera camera;
    if(pos.x()==1.0 && pos.y() == 1.0 && pos.z() ==1.0){
        camera.position= cameraPos + (cameraForward*3);
    }else camera.position= pos;
    camera.rotation= rot;
    camera.angle=angle;
    cameras.push_back(camera);
}

void ModelWindow_GL::createMarker(QVector3D pos,QVector3D rot,float angle){
    Marker marker;
    if(pos.x()==1.0 && pos.y() == 1.0 && pos.z() ==1.0){
        marker.position = cameraPos + (cameraForward*3);
    }else marker.position= pos;
    marker.rotation= rot;
    marker.angle=angle;
    marker.scale=QVector2D(1.0,1.0);
    marker.texture = defaultMarkerTexture;
    marker.image = defaultMarkerImage;

    markers.push_back(marker);
    selectedMarker=markers.size()-1;

    emit glSignalEmitter->editMarker(markers.size()-1,&markers[markers.size()-1]);
}

void ModelWindow_GL::createEntity(){
    QVector3D pos= QVector3D(lastMouseWorldPos[0],lastMouseWorldPos[1],lastMouseWorldPos[2]);
    QVector3D rot= QVector3D(0.0,0.0,0.0);
    float angle=0.0;
    //TODO CREATE MARKER
    switch (createMode) {
        case CREATE_MARKER:
            createMarker(pos,rot,angle);
            break;
        case CREATE_CAMERA:
            createCamera(pos,rot,angle);
            break;
        default:
            break;
    }

    createMode=NONE;
}

void ModelWindow_GL::paintGL()
{
    //if(cameras.size()>0){
        setupCamera(QVector3D(0,0,10),QVector3D(0,0,0),QVector3D(0,1,0));
        RenderPass(CameraSim,true,true,false);
        m_shadow = m_view;
    //}


    //Setup Camera taking into account mouse and key inputs
    setupCamera(cameraPos,cameraPos+cameraForward,upVec);
    //Render Passes
    //RenderPass(Pass,Render to fbo?,render Model?,render Entities(cam + markers)?)
    RenderPass(Normals,true,true,false);
    RenderPass(Depth,true,true,false);
    RenderPass(Picking,true,false,true);
    RenderPass(Full,false,true,true);

    shadowTexture->release();

    //Create Camera or Marker
    if(mouseDClick){
        createEntity();
        mouseDClick=false;
    }
}

void ModelWindow_GL::setShaderUniformNodeValues(QMatrix4x4 objectMatrix){
    QMatrix4x4 modelMatrix = m_model * objectMatrix;
    QMatrix4x4 modelViewMatrix = m_view * modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_projection * modelViewMatrix;
    switch (pass) {
    case Normals:
        m_NormalshaderProgram.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_NormalshaderProgram.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_NormalshaderProgram.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_NormalshaderProgram.setUniformValue( "P", m_projection );
        m_NormalshaderProgram.setUniformValue( "V", m_view );
        break;
    case Depth:
        m_DepthshaderProgram.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_DepthshaderProgram.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_DepthshaderProgram.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_DepthshaderProgram.setUniformValue( "P", m_projection );
        m_DepthshaderProgram.setUniformValue( "V", m_view );
        break;
    case Picking:
        m_ObjectPicking.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_ObjectPicking.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_ObjectPicking.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_ObjectPicking.setUniformValue( "P", m_projection );
        m_ObjectPicking.setUniformValue( "V", m_view );
        break;
    case MarkerTex:
        m_MarkerTextureProgram.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_MarkerTextureProgram.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_MarkerTextureProgram.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_MarkerTextureProgram.setUniformValue( "P", m_projection );
        m_MarkerTextureProgram.setUniformValue( "V", m_view );
        break;
    case Full:{
        QMatrix4x4 shadowModel = QMatrix4x4();
        shadowModel.setToIdentity();
        QMatrix4x4 shadowMVP = m_projection * (m_shadow *shadowModel);

        m_shaderProgram.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_shaderProgram.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_shaderProgram.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_shaderProgram.setUniformValue( "P", m_projection );
        m_shaderProgram.setUniformValue( "V", m_view );
        m_shaderProgram.setUniformValue("shadow", shadowMVP);
        m_shaderProgram.setUniformValue("texture", 0);
        shadowTexture->bind();

        break;}
    case CameraSim:
        m_ShadowMapProgram.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_ShadowMapProgram.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_ShadowMapProgram.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_ShadowMapProgram.setUniformValue( "P", m_projection );
        m_ShadowMapProgram.setUniformValue( "V", m_view );
        break;
    default:
        break;
    }
}

void ModelWindow_GL::drawNode(const Node *node, QMatrix4x4 objectMatrix)
{
    objectMatrix *= node->transformation;
    setShaderUniformNodeValues(objectMatrix);

    // Draw each mesh in this node
    for(int imm = 0; imm<node->meshes.size(); ++imm)
    {
        if(node->meshes[imm]->material->Name == QString("DefaultMaterial"))
            setMaterialUniforms(m_materialInfo);
        else
            setMaterialUniforms(*node->meshes[imm]->material);

        glDrawElements( GL_TRIANGLES, node->meshes[imm]->indexCount, GL_UNSIGNED_INT
                            , (const void*)(node->meshes[imm]->indexOffset * sizeof(unsigned int)) );
    }

    // Recursively draw this nodes children nodes
    for(int inn = 0; inn<node->nodes.size(); ++inn)
        drawNode(&node->nodes[inn], objectMatrix);
}

void ModelWindow_GL::setMaterialUniforms(MaterialInfo &mater)
{
    switch (pass) {
    case Normals:
        m_NormalshaderProgram.setUniformValue( "Ka", mater.Ambient );
        m_NormalshaderProgram.setUniformValue( "Kd", mater.Diffuse );
        m_NormalshaderProgram.setUniformValue( "Ks", mater.Specular );
        m_NormalshaderProgram.setUniformValue( "shininess", mater.Shininess );
        break;
    case Depth:

        m_DepthshaderProgram.setUniformValue( "Ka", mater.Ambient );
        m_DepthshaderProgram.setUniformValue( "Kd", mater.Diffuse );
        m_DepthshaderProgram.setUniformValue( "Ks", mater.Specular );
        m_DepthshaderProgram.setUniformValue( "shininess", mater.Shininess );
        break;
    case Full:
        m_shaderProgram.setUniformValue( "Ka", mater.Ambient );
        m_shaderProgram.setUniformValue( "Kd", mater.Diffuse );
        m_shaderProgram.setUniformValue( "Ks", mater.Specular );
        m_shaderProgram.setUniformValue( "shininess", mater.Shininess );
        break;
    case MarkerTex:
        m_MarkerTextureProgram.setUniformValue( "Ka", mater.Ambient );
        m_MarkerTextureProgram.setUniformValue( "Kd", mater.Diffuse );
        m_MarkerTextureProgram.setUniformValue( "Ks", mater.Specular );
        m_MarkerTextureProgram.setUniformValue( "shininess", mater.Shininess );
        break;
    case Picking:
        m_ObjectPicking.setUniformValue( "Ka", mater.Ambient );
        m_ObjectPicking.setUniformValue( "Kd", mater.Diffuse );
        m_ObjectPicking.setUniformValue( "Ks", mater.Specular );
        m_ObjectPicking.setUniformValue( "shininess", mater.Shininess );
        break;
    case CameraSim:
        m_ShadowMapProgram.setUniformValue( "Ka", mater.Ambient );
        m_ShadowMapProgram.setUniformValue( "Kd", mater.Diffuse );
        m_ShadowMapProgram.setUniformValue( "Ks", mater.Specular );
        m_ShadowMapProgram.setUniformValue( "shininess", mater.Shininess );
    default:
        break;
    }
}

void ModelWindow_GL::cleanupGL()
{
}
