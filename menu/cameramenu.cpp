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

void CameraMenu::editCamera(int index,Camera* camera,bool* runSim)
{
    m_Index=index;
    m_Camera=camera;
    m_runSim=runSim;
    ui->focal->setValue(0.0);
    ui->hei->setValue(0.0);

    ui->len->setValue(0.0);
    ui->width->setValue(0.0);

    connect(ui->simulation,SIGNAL(pressed()),this,SLOT(runSimulation()));

}

void CameraMenu::runSimulation(){
    *m_runSim=true;
}

