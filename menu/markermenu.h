#ifndef MARKERMENU_H
#define MARKERMENU_H


#include <QWidget>
#include <glsurface/openglwindow.h>

namespace Ui {
class MarkerMenu;
}

class MarkerMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MarkerMenu(QWidget *parent = 0);
    void editMarker(int index,Marker* marker);

private:
    Ui::MarkerMenu *ui;
    int m_Index;
    Marker *m_Marker;

public slots:
    void markerEdit();
    void OpenImage();
    void changeName();

signals:
    void markerChanged(int index,Marker* marker);

};


#endif // MARKERMENU_H
