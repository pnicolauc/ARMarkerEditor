#ifndef ARDATALOADER_H
#define ARDATALOADER_H

#include <glsurface/openglwindow.h>
#include <QHash>
#include <QVector2D>

class ARDataLoader
{
public:
    ARDataLoader(QString zipfile=QString(""));

    void extract();
    void compress(QString filename);

    void parseJSonFile();
    void parseXMLFile();
    void save(QString filename);
    bool copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit);

    QVector<Marker> getMarkers();
    void setMarkers(QVector<Marker> mks);

    QVector<Marker> markers;

    QHash<QString,QVector2D> markerSizes;

    QString zipfile;
    QString jsonfile;
    QString xmlfile;
    QString key;
    QString modelFolder;
    QString threeDFile;
    QString realthreeDFile;

    QString fullModelfolder;
    QString fullModelfolder2;

    QString fullVuforiaZippath;
private:


};

#endif // ARDATALOADER_H
