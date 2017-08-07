#include "ardataloader.h"
#include <zipreader/zipreader.h>
#include <zipreader/zipwriter.h>

#include <QFile>
#include <QDir>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <QFileInfo>

#include <QDirIterator>
#include <QMessageBox>

ARDataLoader::ARDataLoader(QString zipfile):zipfile(zipfile)
{
    if(zipfile.length()>0){
        this->extract();
        this->parseJSonFile();
    }
}

void ARDataLoader::extract(){
    ZipReader cZip(zipfile);
    cZip.extractAll("./");
}

void ARDataLoader::addDirToZip(ZipWriter* cZip,QString mainFolder,QString relativePath){
    qDebug() << relativePath;

    QDir dir;
    if(relativePath.length()>0)
        cZip->addDirectory(relativePath);
    dir.setPath(mainFolder + relativePath);

    foreach (QString copy_file, dir.entryList(QDir::Files))
    {
        qDebug() << copy_file;
        QFile file(mainFolder+ relativePath +  copy_file);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open file:");
            return;
        }
        cZip->addFile( relativePath +  copy_file,file.readAll());


        file.close();
    }

    foreach (QString copy_dir, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
    {

        addDirToZip(cZip,mainFolder,relativePath + copy_dir + "/");
    }

}

void ARDataLoader::compress(QString filename){
    ZipWriter cZip(filename);
    addDirToZip(&cZip,"ardata/","");
    cZip.close();
}

bool ARDataLoader::copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit)
{
    QDir dir;
    dir.setPath(from_dir);

    from_dir += QDir::separator();
    to_dir += QDir::separator();

    foreach (QString copy_file, dir.entryList(QDir::Files))
    {
        QString from = from_dir + copy_file;
        QString to = to_dir + copy_file;


        if (QFile::exists(to))
        {
            if (replace_on_conflit)
            {
                if (QFile::remove(to) == false)
                {
                    return false;
                }
            }
            else
            {
                continue;
            }
        }

        if (QFile::copy(from, to) == false)
        {
            return false;
        }
    }

    foreach (QString copy_dir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString from = from_dir + copy_dir;
        QString to = to_dir + copy_dir;

        if (dir.mkpath(to) == false)
        {
            return false;
        }

        if (copy_dir_recursive(from, to, replace_on_conflit) == false)
        {
            return false;
        }
    }

    return true;
}

void ARDataLoader::save(QString filename){
    foreach(Marker m, markers){
        if(m.name.length()==0){
            qDebug()  << markers.size();
            QMessageBox Msgbox;
            Msgbox.setText("All Markers need to have Names.");
            Msgbox.exec();
            return;
        }
    }

    QJsonObject datasetJson;
    datasetJson["key"] = key;
    datasetJson["modelFolder"] = modelFolder;
    datasetJson["real"] =realthreeDFile;
    datasetJson["virtual"] =threeDFile;
    datasetJson["scale"] = 1.0;

    if(xmlfile.length()>0)
        datasetJson["xml"] = xmlfile;
    else if(zipfile.length()>0){
        ZipReader cZip(zipfile);

        cZip.extractAll("ardata");

        QList<ZipReader::FileInfo> allFiles = cZip.fileInfoList();
        foreach (ZipReader::FileInfo fi, allFiles) {

            if(fi.filePath.endsWith("xml")){
                datasetJson["xml"] = fi.filePath;
                xmlfile=fi.filePath;
            }
        }
    }

    writeXMLFile();
    QDir arDir("ardata/model");
    if (!QDir().mkpath(arDir.absolutePath()))
        return;

    if(!fullModelfolder.endsWith("ardata/"+modelFolder))
        copy_dir_recursive(fullModelfolder,"ardata/"+ modelFolder,true);
    if(!fullModelfolder2.endsWith("ardata/"+modelFolder))
        copy_dir_recursive(fullModelfolder2,"ardata/"+modelFolder,true);

    QJsonArray mkArray;
    foreach (Marker mk, markers) {
        QJsonObject mkObj;
        mkObj["name"]=mk.name;

        QJsonArray rotArray;
        rotArray.append(mk.angle);
        rotArray.append(mk.rotation[0]);
        rotArray.append(mk.rotation[1]);
        rotArray.append(mk.rotation[2]);
        mkObj["rotation"]=rotArray;

        QJsonArray trArray;
        trArray.append(mk.position[0]);
        trArray.append(mk.position[1]);
        trArray.append(mk.position[2]);
        mkObj["translation"]=trArray;

        QJsonArray scArray;
        scArray.append(mk.scale[0]);
        scArray.append(mk.scale[1]);
        scArray.append(mk.scale[2]);
        mkObj["scale"]=scArray;

        mkArray.append(mkObj);
    }

    datasetJson["markers"]=mkArray;

    QJsonDocument saveJson(datasetJson);
    QFile datasetFile("ardata/dataset.json");

    if (!datasetFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open json file.");
        return;
    }
    datasetFile.write(saveJson.toJson());

    datasetFile.close();

    compress(filename);
}

void ARDataLoader::parseJSonFile(){
    QFile jsonFile(QDir::currentPath() + QString("/ardata/dataset.json"));

    if (!jsonFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open json file.");
        return;
    }
    QByteArray jsonData = jsonFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(jsonData));

    QJsonObject json= loadDoc.object();

    modelFolder = json["modelFolder"].toString();
    realthreeDFile = json["real"].toString();
    threeDFile = json["virtual"].toString();

    xmlfile = json["xml"].toString();
    key = json["key"].toString();

    this->parseXMLFile();

    QJsonArray markerArray = json["markers"].toArray();

    for (int markerIndex = 0; markerIndex < markerArray.size(); ++markerIndex) {
        QJsonObject markerObject = markerArray[markerIndex].toObject();
        Marker marker;
        marker.name = markerObject["name"].toString();
        QJsonArray rotationArray = markerObject["rotation"].toArray();
        QJsonArray translationArray = markerObject["translation"].toArray();
        marker.angle= rotationArray[0].toDouble();
        marker.rotation[0]= rotationArray[1].toDouble();
        marker.rotation[1]= rotationArray[2].toDouble();
        marker.rotation[2]= rotationArray[3].toDouble();

        marker.position[0]= translationArray[0].toDouble();
        marker.position[1]= translationArray[1].toDouble();
        marker.position[2]= translationArray[2].toDouble();
        marker.scale = markerSizes.value(marker.name);

        markers.push_back(marker);

        qDebug() <<  marker.name << marker.angle << " " << marker.position << marker.rotation << marker.scale;
    }
}

void ARDataLoader::writeXMLFile(){
    QFile xmlFile(QDir::currentPath() + QString("/ardata/") + xmlfile);
    if (!xmlFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open xml file.");
        return;
    }
    QXmlStreamWriter* stream = new QXmlStreamWriter(&xmlFile);
    stream->setAutoFormatting(true);

    stream->writeStartDocument();

    stream->writeStartElement("QCARConfig");
    stream->writeStartElement("Tracking");

    foreach(Marker m, markers){
        stream->writeStartElement("ImageTarget");
        stream->writeAttribute("name", m.name);
        stream->writeAttribute("size", QString::number(m.scale[0]) + " " + QString::number(m.scale[1]));
        stream->writeEndElement();
    }

    stream->writeEndElement();
    stream->writeEndElement();
    stream->writeEndDocument();
}

void ARDataLoader::parseXMLFile(){
    QFile xmlFile(QDir::currentPath() + QString("/ardata/") + xmlfile);
    if (!xmlFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open xml file.");
        return;
    }
    QXmlStreamReader* xmlReader = new QXmlStreamReader(&xmlFile);

    //Parse the XML until we reach end of it
    while(!xmlReader->atEnd() && !xmlReader->hasError()) {
        // Read next element
        QXmlStreamReader::TokenType token = xmlReader->readNext();
        //If token is just StartDocument - go to next
        if(token == QXmlStreamReader::StartDocument) {
                continue;
        }
        //If token is StartElement - read it
        if(token == QXmlStreamReader::StartElement) {
            if(xmlReader->name() == "ImageTarget") {
                QString nameID;
                QVector2D xySize;
                foreach(const QXmlStreamAttribute &attr, xmlReader->attributes()) {
                    if (attr.name() == "name") {
                        nameID=attr.value().toString();
                    }else if(attr.name() == "size"){
                        QStringList xyStrings = attr.value().toString().split(" ");
                        for(int i=0;i<xyStrings.size();i++){
                            xySize[i]=xyStrings[i].toDouble();
                        }
                    }

                }
                markerSizes.insert(nameID,xySize);
            }
        }
    }

    xmlReader->clear();
    xmlFile.close();
}

QVector<Marker> ARDataLoader::getMarkers(){
    return markers;
}
void ARDataLoader::setMarkers(QVector<Marker> mks){
    markers=mks;
}


