#ifndef SAVEPROJECT_H
#define SAVEPROJECT_H

#include <QDialog>

namespace Ui {
class SaveProject;
}

class SaveProject : public QDialog
{
    Q_OBJECT

public:
    explicit SaveProject(QWidget *parent = 0);
    ~SaveProject();
    Ui::SaveProject *ui;

    QUrl selectFile(QString title,QString filters);
    void setModelFolder(QString path);
    void setSavePath(QString path);
    void setVufData(QString path);
    void setVirtualFolder(QString path);
    void setKey(QString key);

    QString xml;

private:
    QString modelFolder;
    QString zipFolder;
    QString outPath;
    QString virtualFolder;
    QString m_key;
public slots:
    void save();
    void browse1();
    void browse2();
    void browse3();
    void browse4();

signals:
    void saveProj(QString mf,QString vf, QString zf, QString op,QString key);
};

#endif // SAVEPROJECT_H
