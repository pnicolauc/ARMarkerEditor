#include "markermenu.h"
#include "ui_markermenu.h"
#include <QFileDialog>
#include <QtDebug>
MarkerMenu::MarkerMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarkerMenu)
{
    ui->setupUi(this);

    this->setMinimumSize(500, 500);
    this->setMaximumSize(1000, 1000);
    this->setFocusPolicy(Qt::TabFocus);

}

void MarkerMenu::editMarker(int index,Marker *marker){
    ui->posx->setValue(marker->position.x());
    ui->posy->setValue(marker->position.y());
    ui->posz->setValue(marker->position.z());

    ui->height->setValue(marker->scale.y());
    ui->width->setValue(marker->scale.x());

    ui->rotx->setValue(marker->rotation.x());
    ui->roty->setValue(marker->rotation.y());
    ui->rotz->setValue(marker->rotation.z());

    ui->ang->setValue(marker->angle);

    ui->texImage->setPixmap(QPixmap::fromImage((marker->image->mirrored())));
    ui->texImage->setScaledContents( true );
    m_Index = index;
    m_Marker= marker;

    connect(ui->posx,SIGNAL(valueChanged(double)),this,SLOT(markerEdit()));
    connect(ui->posy,SIGNAL(valueChanged(double)),this,SLOT(markerEdit()));
    connect(ui->posz,SIGNAL(valueChanged(double)),this,SLOT(markerEdit()));
    connect(ui->width,SIGNAL(valueChanged(double)),this,SLOT(markerEdit()));
    connect(ui->height,SIGNAL(valueChanged(double)),this,SLOT(markerEdit()));

    connect(ui->rotx,SIGNAL(valueChanged(double)),this,SLOT(markerEdit()));
    connect(ui->roty,SIGNAL(valueChanged(double)),this,SLOT(markerEdit()));
    connect(ui->rotz,SIGNAL(valueChanged(double)),this,SLOT(markerEdit()));
    connect(ui->ang,SIGNAL(valueChanged(int)),this,SLOT(markerEdit()));

    connect(ui->filePicker,SIGNAL(pressed()),this,SLOT(OpenImage()));
}

void MarkerMenu::OpenImage(){
    QString sourceFileName;
    QWidget *container = new QWidget();
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    sourceFileName = dialog.getOpenFileName(container, QStringLiteral("Open an Image"));

    if(sourceFileName.length()>0){
        QImage* qImage= new QImage();
        qImage->load(sourceFileName);

        ui->texImage->setPixmap(QPixmap::fromImage(*qImage));

        m_Marker->image = qImage;
        m_Marker->texture = new QOpenGLTexture(m_Marker->image->mirrored());

        markerEdit();
    }
}

void MarkerMenu::markerEdit(){
    m_Marker->position.setX(ui->posx->value());
    m_Marker->position.setY(ui->posy->value());
    m_Marker->position.setZ(ui->posz->value());

    m_Marker->scale.setX(ui->width->value());
    m_Marker->scale.setY(ui->height->value());

    m_Marker->rotation.setX(ui->rotx->value());
    m_Marker->rotation.setY(ui->roty->value());
    m_Marker->rotation.setZ(ui->rotz->value());

    m_Marker->angle= ui->ang->value();
    //emit markerChanged(m_Index,m_Marker);
}


