#include "saveproject.h"
#include "ui_saveproject.h"

#include <QFileDialog>

SaveProject::SaveProject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveProject)
{
    ui->setupUi(this);
    this->setMinimumSize(500, 500);
    this->setMaximumSize(1000, 1000);
    this->setFocusPolicy(Qt::TabFocus);

    connect(ui->browser1,SIGNAL(pressed()),this,SLOT(browse1()));
    connect(ui->browser2,SIGNAL(pressed()),this,SLOT(browse2()));
    connect(ui->browser3,SIGNAL(pressed()),this,SLOT(browse3()));
    connect(ui->browser4,SIGNAL(pressed()),this,SLOT(browse4()));

    connect(ui->ui_save,SIGNAL(pressed()),this,SLOT(save()));
}

SaveProject::~SaveProject()
{
    delete ui;
}

void SaveProject::setModelFolder(QString path){
    modelFolder=path;
    ui->model_path->setText(modelFolder);

}

void SaveProject::setVirtualFolder(QString path){
    virtualFolder=path;
    ui->virtual_folder->setText(virtualFolder);

}
void SaveProject::setSavePath(QString path){
    outPath= path;
    ui->save_path->setText(outPath);
}
void SaveProject::setVufData(QString path){
    zipFolder=path;
    ui->vuf_data->setText(zipFolder);
}

void SaveProject::setKey(QString key){
    m_key=key;
    ui->vuf_key->setText(key);
}

void SaveProject::save(){
    emit saveProj(modelFolder,virtualFolder,zipFolder,outPath,m_key);
}

void SaveProject::browse1(){
    outPath= QFileDialog::getSaveFileName(this, "Save file", "", ".ardata");
    ui->save_path->setText(outPath);

}

void SaveProject::browse4(){
    virtualFolder= QFileDialog::getSaveFileName(this, "Choose Virtual 3D Model Folder", "", ".ardata");
    ui->virtual_folder->setText(virtualFolder);

}

void SaveProject::browse2(){
    modelFolder=QFileDialog::getExistingDirectory (this, QString("Choose 3D Model Folder") );
    ui->model_path->setText(modelFolder);
}


QUrl SaveProject::selectFile(QString title,QString filters){
    QUrl sourceFileName;
    QWidget *container = new QWidget();
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    sourceFileName = dialog.getOpenFileUrl(container, title,QString(""),filters);

    return sourceFileName;
}


void SaveProject::browse3(){
    QString fileFormat="ZIP (*.zip)";
    zipFolder=selectFile("Select Vuforia Data file", fileFormat).path().remove(0,1);
    ui->vuf_data->setText(zipFolder);
}

