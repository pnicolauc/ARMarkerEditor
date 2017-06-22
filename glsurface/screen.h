#ifndef SCREEN_H
#define SCREEN_H

#endif // SCREEN_H
#include <QOpenGLFunctions_3_3_Core>

struct Screen{
    int height;
    int width;
    int mouseSelectedX,mouseSelectedY;
    int lastMouseX,lastMouseY;

    bool mouseDClick;
    bool mouseDrag;
    bool mouseRightDrag;


    QMatrix4x4 m_projection;
};
