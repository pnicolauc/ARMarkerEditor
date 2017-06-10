#include <QApplication>
#include <QWidget>
#include <QFileDialog>
#include <menu/mainwindow.h>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
