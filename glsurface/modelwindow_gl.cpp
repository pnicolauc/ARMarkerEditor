#include "modelwindow_gl.h"
#include <QOpenGLFramebufferObject>
#include <QVector>
#include <QtCore/qmath.h>
#include <stdio.h>      /* printf */

#include <math.h>       /* acos */


ModelWindow_GL::ModelWindow_GL(QString filepath, ModelLoader::PathType pathType, QString texturePath) :
    OpenGLWindow()
  , m_indexBuffer(QOpenGLBuffer::IndexBuffer)
  , m_filepath(filepath)
  , m_pathType(pathType)
  , m_texturePath(texturePath)
  , m_error(false)
{
}

void ModelWindow_GL::initializeGL()
{
    viewCam.shiftPressed=false;
    simDirs.push_back( QVector3D(0,0,-1));
    simDirs.push_back( QVector3D(0,0,1));
    simDirs.push_back( QVector3D(1,0,0));
    simDirs.push_back( QVector3D(-1,0,0));
    simDirs.push_back( QVector3D(0,1,0));
    simDirs.push_back( QVector3D(0,-1,0));

    screen.mouseDrag=false;
    screen.mouseDClick=false;
    cameraSim = false;
    entities.runSim = false;

    createMode= NONE;
    create_mode = false;
    simCount=0;

    entities = Entities();
    viewCam = ViewCamera();
    shaders = Shaders();
    scroll=0;
    selectedMarker=-1;
    this->initializeOpenGLFunctions();

    float aspect = 1.0f;
    screen.m_projection.setToIdentity();
    screen.m_projection.perspective(
                60.0f,          // field of vision
                aspect,         // aspect ratio
                0.5f,           // near clipping plane
                100.0f);       // far clipping plane

    createBuffers();
    createShaderProgram(":/resources/shaders/ads_fragment.vert", ":/resources/shaders/ads_fragment.frag",&m_shaderProgram);
    createShaderProgram(":/resources/shaders/normals.vert", ":/resources/shaders/normals.frag",&m_NormalshaderProgram);
    createShaderProgram(":/resources/shaders/depth.vert", ":/resources/shaders/depth.frag",&m_DepthshaderProgram);
    createShaderProgram(":/resources/shaders/markertexture.vert", ":/resources/shaders/markertexture.frag",&m_MarkerTextureProgram);
    createShaderProgram(":/resources/shaders/picking.vert", ":/resources/shaders/picking.frag",&m_ObjectPicking);
    createShaderProgram(":/resources/shaders/shadow.vert", ":/resources/shaders/shadow.frag",&m_ShadowMapProgram);
    createShaderProgram(":/resources/shaders/markerDepth.vert", ":/resources/shaders/markerDepth.frag",&m_MarkerDepthProgram);

    createAttributes(&m_shaderProgram);
    createAttributes(&m_NormalshaderProgram);
    createAttributes(&m_DepthshaderProgram);
    createAttributes(&m_MarkerTextureProgram);
    createAttributes(&m_ObjectPicking);
    createAttributes(&m_ShadowMapProgram);
    createAttributes(&m_MarkerDepthProgram);

    setupLightingAndMatrices();

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
void ModelWindow_GL::createAttributes(QOpenGLShaderProgram* shaderprogram){
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



void ModelWindow_GL::createBuffers()
{
    ModelLoader model;

    #ifdef WIN32
    m_filepath.remove(0,1);
    #endif

    qDebug() << m_filepath;
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


void ModelWindow_GL::setupLightingAndMatrices()
{
    m_lightInfo.Position = QVector4D( -1.0f, 1.0f, 1.0f, 1.0f );
    m_lightInfo.Intensity = QVector3D( 1.0f, 1.0f, 1.0f);

    m_materialInfo.Ambient = QVector3D( 0.05f, 0.2f, 0.05f );
    m_materialInfo.Diffuse = QVector3D( 0.3f, 0.5f, 0.3f );
    m_materialInfo.Specular = QVector3D( 0.6f, 0.6f, 0.6f );
    m_materialInfo.Shininess = 50.0f;
}

void ModelWindow_GL::resizeGL(int w, int h)
{
    glViewport( 0, 0, w, h );
    normsTex = new GLfloat[4*w*h];
    depthTex = new GLfloat[4*w*h];
    screen.height=h;
    screen.width=w;
    screen.m_projection.setToIdentity();
    screen.m_projection.perspective(60.0f, (float)w/h, .3f, 100);
}

void ModelWindow_GL::drawCameras(){
    for(int i=0;i< entities.cameraCount();i++){
        Camera cam= entities.getCamera(i);
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
        curr_Program=getCurrentProgram();
        shaders.bindProgram(curr_Program,pass,m_lightInfo);
    }
    for(int i=0;i< entities.markerCount();i++){
        Marker mk= entities.getMarker(i);
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
    if(pass==MarkerTex){
        if(createMode==CREATE_MARKER){
            create_mode = true;

            /*Marker tempMk = entities.createTemporaryMarker(
                        lastMouseWorldNormals,lastMouseWorldPos);

            tempMk.texture->bind();
            m_model.setToIdentity();
            m_model.translate(tempMk.position);
            m_model.rotate(tempMk.angle,tempMk.rotation);
            m_model.scale(tempMk.scale.x(),tempMk.scale.y(),0.0);

            m_MarkerTextureProgram.setUniformValue( "selected", true );
            m_vao.bind();
            drawNode(m_markerNode.data(), QMatrix4x4());
            m_vao.release();*/
        }
    }

    m_model.setToIdentity();
}

void ModelWindow_GL::setupRenderTarget(){
    switch(pass){
        case Depth:
        case Normals:
        case CameraSim:

        if(float_fbo==nullptr){
                        QOpenGLFramebufferObjectFormat format2;
                        float_fbo = new QOpenGLFramebufferObject(screen.width,screen.height, QOpenGLFramebufferObject::CombinedDepthStencil,
                                                                 GL_TEXTURE_2D,
                                                                 GL_RGBA32F);
        }
        float_fbo->bind();
            break;
        case Picking:
        if(int_fbo==nullptr){
                        QOpenGLFramebufferObjectFormat format;
                        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
                        int_fbo = new QOpenGLFramebufferObject(screen.width,screen.height, format);
        }
        int_fbo->bind();
        break;
    }
}
void ModelWindow_GL::releaseRenderTarget(){
    QImage simImg;
    switch (pass) {
        case Picking:
            fboPickingImage = int_fbo->toImage();
            fboPickingImage.save("fbo.png");
            break;
        case Depth:
                glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
                glReadPixels(screen.lastMouseX, screen.height-screen.lastMouseY, 1, 1, GL_RGBA, GL_FLOAT, lastMouseWorldPos);
                qDebug() << "Position" << " " << lastMouseWorldPos[2]<< " " <<lastMouseWorldPos[0] << " " << lastMouseWorldPos[1] << " " << lastMouseWorldPos[2];
            break;
        case Normals:
                glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
                glReadPixels(screen.lastMouseX, screen.height-screen.lastMouseY, 1, 1, GL_RGBA, GL_FLOAT, lastMouseWorldNormals);
                //qDebug() << "Normals:" <<lastMouseWorldNormals[0] << " " << lastMouseWorldNormals[1] << " " << lastMouseWorldNormals[2];
            break;
        case CameraSim:
        switch (simCount) {
            case 0:
                simImg = float_fbo->toImage();
                sim0= float_fbo->takeTexture();
                m_SimView0 = QMatrix4x4(viewCam.getViewM());
                break;
            case 1:
                simImg = float_fbo->toImage();
                sim1= float_fbo->takeTexture();
                m_SimView1 = QMatrix4x4(viewCam.getViewM());

            case 2:
                simImg = float_fbo->toImage();
                sim2= float_fbo->takeTexture();
                m_SimView2 = QMatrix4x4(viewCam.getViewM());

            case 3:
                simImg = float_fbo->toImage();
                sim3= float_fbo->takeTexture();
                m_SimView3 = QMatrix4x4(viewCam.getViewM());

            case 4:
                simImg = float_fbo->toImage();
                sim4= float_fbo->takeTexture();
                m_SimView4 = QMatrix4x4(viewCam.getViewM());

            case 5:
                simImg = float_fbo->toImage();
                sim5= float_fbo->takeTexture();
                m_SimView5 = QMatrix4x4(viewCam.getViewM());

            default:
            break;
        }
            break;
    }
}
QOpenGLShaderProgram* ModelWindow_GL::getCurrentProgram(){
    switch(pass){
        case Normals:return &m_NormalshaderProgram;
        case Depth:return &m_DepthshaderProgram;
        case Picking:return &m_ObjectPicking;
        case MarkerTex:return &m_MarkerTextureProgram;
        case CameraSim:return &m_ShadowMapProgram;
        case Full:return &m_shaderProgram;
    }
}

void ModelWindow_GL::RenderPass(Pass currPass,bool renderFBO,bool renderModel,bool renderEntities ){
    if(m_error)
        return;

    pass=currPass;
    curr_Program=getCurrentProgram();
    // Clear color and depth buffers
    // Bind shader programs and rendertargets
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    shaders.bindProgram(curr_Program,pass,m_lightInfo);

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
    if(event->button() == Qt::LeftButton){
        screen.mouseDrag=true;
    }else if(event->button() == Qt::RightButton){
        screen.mouseRightDrag=true;
    }

}
void ModelWindow_GL::mouseReleaseEvent(QMouseEvent* event){
    if(event->button() == Qt::LeftButton){
        screen.mouseDrag= false;
    }
    else if(event->button() == Qt::RightButton){
        screen.mouseRightDrag=false;
    }
}
void ModelWindow_GL::mouseMoveEvent(QMouseEvent* event){
    if(screen.mouseDrag)
    {
        float difX = (screen.lastMouseX - event->x())/100.0;
        float difY = (screen.lastMouseY - event->y())/100.0;

        viewCam.mouseRotate(difX,difY);

    }
    screen.lastMouseX=event->x();
    screen.lastMouseY=event->y();

}

void ModelWindow_GL::mouseDoubleClickEvent(QMouseEvent* event)
{
    QColor color = QColor(fboPickingImage.pixel(event->x(),event->y()));
    if(color.red()!=255) {
        qDebug() << "Clicked on Marker";
        selectedMarker = color.red();
       // emit glSignalEmitter->editMarker(selectedMarker,&markers[selectedMarker]);
    }else{
        screen.mouseSelectedX=event->x();
        screen.mouseSelectedY=event->y();
        screen.mouseDClick=true;
    }
}

void ModelWindow_GL::wheelEvent(QWheelEvent *event)
{
    scroll += event->delta() / 400.0;

    event->accept();
}
void ModelWindow_GL::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case  Qt::Key_Shift:
            viewCam.shiftPressed=false;
    }
}
void ModelWindow_GL::keyPressEvent(QKeyEvent * ev) {
    switch (ev->key()) {
    case  Qt::Key_W:
        viewCam.walkForward();
        break;
    case Qt::Key_S:
        viewCam.walkBackward();
        break;
    case Qt::Key_C:{
        createMode=CREATE_CAMERA;
         // Set the cursor target
        qDebug() << "CREATE_CAMERA";
        break;
        }
    case Qt::Key_M:{
        // Create a target cursor from a resource file
        createMode=CREATE_MARKER;
        qDebug() << "CREATE_MARKER";

        break;
        }
    case Qt::Key_Shift:
        viewCam.shiftPressed=true;
        break;

    default:
        break;
    }

    this->setCursor(entities.getCursor(createMode));

}

void ModelWindow_GL::markerChanged(int index,Marker* marker){
   // markers[index]=*markerr;
}

void ModelWindow_GL::createEntity(){
    QVector3D pos;

    QVector3D rot;
    float angle=0.0;

    switch (createMode) {
        case CREATE_MARKER:{
            QVector3D defRot=QVector3D(0.0,0.0,-1.0);
            QVector3D normalV = QVector3D(lastMouseWorldNormals[0],lastMouseWorldNormals[1],lastMouseWorldNormals[2]);
            QVector3D cross= QVector3D::crossProduct(defRot,normalV);
            float dot= QVector3D::dotProduct(defRot,normalV);
            float angle= acos (dot);

            pos= QVector3D(lastMouseWorldPos[0],lastMouseWorldPos[1],lastMouseWorldPos[2]);
            rot= cross;
            selectedMarker=entities.createMarker(pos,rot,angle*57.2957795);
            create_mode=false;
    }
            break;
        case CREATE_CAMERA:
            pos= QVector3D(lastMouseWorldPos[0]+lastMouseWorldNormals[0],
                    lastMouseWorldPos[1]+lastMouseWorldNormals[1],
                    lastMouseWorldPos[2]+lastMouseWorldNormals[2]);


            rot= QVector3D(0.0,0.0,0.0);
            entities.createCamera(pos,rot,angle);
            break;
        default:
            break;
    }
    createMode=NONE;
    this->setCursor(entities.getCursor(createMode));

}

void ModelWindow_GL::paintGL()
{
    //Setup Camera taking into account mouse and key inputs
    viewCam.setupCamera();
    //Render Passes
    //RenderPass(Pass,Render to fbo?,render Model?,render Entities(cam + markers)?)
    RenderPass(Normals,true,true,false);
    RenderPass(Depth,true,true,false);
    RenderPass(Picking,true,false,true);

    if(entities.runSim){
        entities.simIndex=entities.selectedCam;

        QVector3D defUp= QVector3D(0,1,0);
        QVector3D vertUp= QVector3D(0,0,1);
        QVector3D currUp;
        if(simCount<4) currUp = defUp; else currUp= vertUp;
        viewCam.setupCamera(entities.getCamera(entities.simIndex).position,
                            entities.getCamera(entities.simIndex).position- simDirs[simCount],
                            currUp);
        RenderPass(CameraSim,true,true,false);

        simCount++;
        if(simCount==6){
            cameraSim=true;
            entities.runSim=false;
        }
    }

    viewCam.setupCamera();

    RenderPass(Full,false,true,true);

    //shadowTexture->release();

    //Create Camera or Marker
    if(screen.mouseDClick){
        createEntity();
        screen.mouseDClick=false;
    }
    if(screen.mouseRightDrag && selectedMarker>=0){
        QVector3D defRot=QVector3D(0.0,0.0,-1.0);
        QVector3D normalV = QVector3D(lastMouseWorldNormals[0],lastMouseWorldNormals[1],lastMouseWorldNormals[2]);
        QVector3D cross= QVector3D::crossProduct(defRot,normalV);
        float dot= QVector3D::dotProduct(defRot,normalV);

        float angle= acos (dot);

        if(lastMouseWorldPos[0]==1 && lastMouseWorldPos[1]==1 && lastMouseWorldPos[2]==1)
           entities.getMarkerPtr(selectedMarker)->position=viewCam.getForwardPos();
        else
           entities.getMarkerPtr(selectedMarker)->position=QVector3D(lastMouseWorldPos[0],lastMouseWorldPos[1],lastMouseWorldPos[2]);
       if(scroll!=0){
           entities.getMarkerPtr(selectedMarker)->position+=QVector3D(scroll*lastMouseWorldNormals[0],
                                                                   scroll*lastMouseWorldNormals[1],
                                                                   scroll*lastMouseWorldNormals[2]);
       }

       qDebug() << entities.getMarkerPtr(selectedMarker)->position;
       entities.getMarkerPtr(selectedMarker)->rotation=cross;
       entities.getMarkerPtr(selectedMarker)->angle=angle*57.2957795;

       entities.emitChangedMarkerSignal(selectedMarker);

    }

}

void ModelWindow_GL::setShaderUniformNodeValues(QMatrix4x4 objectMatrix){
    QMatrix4x4 modelMatrix = m_model * objectMatrix;
    QMatrix4x4 modelViewMatrix = viewCam.getViewM() * modelMatrix;
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp;
    if(pass!=CameraSim)
        mvp = screen.m_projection * modelViewMatrix;
    else mvp = entities.getCubeMapProjectionMatrix() * modelViewMatrix;
    switch (pass) {
    case Normals:
        m_NormalshaderProgram.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_NormalshaderProgram.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_NormalshaderProgram.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_NormalshaderProgram.setUniformValue( "P", screen.m_projection );
        m_NormalshaderProgram.setUniformValue( "V", viewCam.getViewM() );
        break;
    case Depth:
        m_DepthshaderProgram.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_DepthshaderProgram.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_DepthshaderProgram.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_DepthshaderProgram.setUniformValue( "P", screen.m_projection );
        m_DepthshaderProgram.setUniformValue( "V", viewCam.getViewM() );
        break;
    case Picking:
        m_ObjectPicking.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_ObjectPicking.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_ObjectPicking.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_ObjectPicking.setUniformValue( "P", screen.m_projection );
        m_ObjectPicking.setUniformValue( "V", viewCam.getViewM() );
        break;
    case MarkerTex:
        m_MarkerTextureProgram.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_MarkerTextureProgram.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_MarkerTextureProgram.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_MarkerTextureProgram.setUniformValue( "P", screen.m_projection );
        m_MarkerTextureProgram.setUniformValue( "V", viewCam.getViewM() );
        m_MarkerTextureProgram.setUniformValue( "create_mode", create_mode );

        break;
    case Full:{
        QMatrix4x4 sim_mvp0 = entities.getCubeMapProjectionMatrix() * (m_SimView0 *modelMatrix);
        QMatrix4x4 sim_mvp1 = entities.getCubeMapProjectionMatrix() * (m_SimView1 *modelMatrix);
        QMatrix4x4 sim_mvp2 = entities.getCubeMapProjectionMatrix() * (m_SimView2 *modelMatrix);
        QMatrix4x4 sim_mvp3 = entities.getCubeMapProjectionMatrix() * (m_SimView3 *modelMatrix);
        QMatrix4x4 sim_mvp4 = entities.getCubeMapProjectionMatrix() * (m_SimView4 *modelMatrix);
        QMatrix4x4 sim_mvp5 = entities.getCubeMapProjectionMatrix() * (m_SimView5 *modelMatrix);

        m_shaderProgram.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_shaderProgram.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_shaderProgram.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_shaderProgram.setUniformValue( "P", screen.m_projection );
        m_shaderProgram.setUniformValue( "V", viewCam.getViewM() );
        if(cameraSim){
            m_shaderProgram.setUniformValue("sim_mvp0",sim_mvp0 );
            m_shaderProgram.setUniformValue("sim_mvp1",sim_mvp1 );
            m_shaderProgram.setUniformValue("sim_mvp2",sim_mvp2 );
            m_shaderProgram.setUniformValue("sim_mvp3",sim_mvp3 );
            m_shaderProgram.setUniformValue("sim_mvp4",sim_mvp4 );
            m_shaderProgram.setUniformValue("sim_mvp5",sim_mvp5 );
            m_shaderProgram.setUniformValue("simTexture0",0);
            m_shaderProgram.setUniformValue("simTexture1",1);
            m_shaderProgram.setUniformValue("simTexture2",2);
            m_shaderProgram.setUniformValue("simTexture3",3);
            m_shaderProgram.setUniformValue("simTexture4",4);
            m_shaderProgram.setUniformValue("simTexture5",5);
            glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
            glBindTexture(GL_TEXTURE_2D, sim0);

            glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
            glBindTexture(GL_TEXTURE_2D, sim1);

            glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
            glBindTexture(GL_TEXTURE_2D, sim2);

            glActiveTexture(GL_TEXTURE0 + 3); // Texture unit 3
            glBindTexture(GL_TEXTURE_2D, sim3);

            glActiveTexture(GL_TEXTURE0 + 4); // Texture unit 4
            glBindTexture(GL_TEXTURE_2D, sim4);

            glActiveTexture(GL_TEXTURE0 + 5); // Texture unit 5
            glBindTexture(GL_TEXTURE_2D, sim5);

        }

        m_shaderProgram.setUniformValue( "cameraSim", cameraSim );


        break;}
    case CameraSim:{
        m_ShadowMapProgram.setUniformValue( "M", m_model );// Transforming to eye space

        m_ShadowMapProgram.setUniformValue( "MV", modelViewMatrix );// Transforming to eye space
        m_ShadowMapProgram.setUniformValue( "N", normalMatrix );    // Transform normal to Eye space
        m_ShadowMapProgram.setUniformValue( "MVP", mvp );           // Matrix for transforming to Clip space
        m_ShadowMapProgram.setUniformValue( "P", entities.getCubeMapProjectionMatrix() );
        m_ShadowMapProgram.setUniformValue( "V", viewCam.getViewM() );

        m_ShadowMapProgram.setUniformValue( "lightPos", entities.getCamera( entities.simIndex).position );


        break;}
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
            shaders.setMaterialUniforms(curr_Program,m_materialInfo);
        else
            shaders.setMaterialUniforms(curr_Program,*node->meshes[imm]->material);

        glDrawElements( GL_TRIANGLES, node->meshes[imm]->indexCount, GL_UNSIGNED_INT
                            , (const void*)(node->meshes[imm]->indexOffset * sizeof(unsigned int)) );
    }

    // Recursively draw this nodes children nodes
    for(int inn = 0; inn<node->nodes.size(); ++inn)
        drawNode(&node->nodes[inn], objectMatrix);
}

void ModelWindow_GL::cleanupGL()
{
}
