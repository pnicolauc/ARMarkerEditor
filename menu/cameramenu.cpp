#include "cameramenu.h"
#include "ui_cameramenu.h"

CameraMenu::CameraMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraMenu)
{
    ui->setupUi(this);
}

CameraMenu::~CameraMenu()
{
    delete ui;
}

void CameraMenu::editCamera(int index,Camera* camera,bool* runSim,CameraParams* camParams,int* simCount)
{
    m_Index=index;
    m_Camera=camera;
    m_CameraParams=camParams;
    m_runSim=runSim;
    m_simCount=simCount;
    ui->haov->setValue(camParams->horizontalAOV);
    ui->vaov->setValue(camParams->verticalAOV);

    m_simCount=simCount;

    ui->hei->setValue(camParams->height);

    ui->len->setValue(camera->scale.x());
    ui->width->setValue(camera->scale.y());

    connect(ui->haov,SIGNAL(valueChanged(double)),this,SLOT(changeCameraParams()));
    connect(ui->vaov,SIGNAL(valueChanged(double)),this,SLOT(changeCameraParams()));
    connect(ui->hei,SIGNAL(valueChanged(double)),this,SLOT(changeCameraParams()));

    connect(ui->len,SIGNAL(valueChanged(double)),this,SLOT(changeCamera()));
    connect(ui->width,SIGNAL(valueChanged(double)),this,SLOT(changeCamera()));

    connect(ui->simulation,SIGNAL(pressed()),this,SLOT(runSimulation()));

}

void CameraMenu::changeCameraParams(){
    m_CameraParams->horizontalAOV=ui->haov->value();
    m_CameraParams->verticalAOV=ui->vaov->value();
    m_CameraParams->height=ui->hei->value();

    m_CameraParams->projection.setToIdentity();
    m_CameraParams->projection.perspective(m_CameraParams->verticalAOV, m_CameraParams->horizontalAOV/m_CameraParams->verticalAOV, .3f, 100);

}

void CameraMenu::changeCamera(){
    m_Camera->scale = QVector2D(ui->len->value(),ui->width->value());
}


void CameraMenu::runSimulation(){
    *m_runSim=true;
    *m_simCount=0;
}

