#-------------------------------------------------
#
# Project created by QtCreator 2017-06-10T18:57:19
#
#-------------------------------------------------

QT       += core gui
CONFIG      += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += core gui widgets quick

lessThan(QT_MAJOR_VERSION, 5): error(This project requires Qt 5 or later)


TARGET = ARMarkerEditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    glsurface/glsignalemitter.cpp \
    glsurface/modelloader.cpp \
    glsurface/openglwindow.cpp \
    menu/mainwindow.cpp \
    menu/markermenu.cpp \
    glsurface/modelwindow_gl.cpp

HEADERS  += glsurface/glsignalemitter.h \
    glsurface/modelloader.h \
    glsurface/openglwindow.h \
    menu/mainwindow.h \
    menu/markermenu.h \
    glsurface/modelwindow_gl.h

FORMS    += menu/mainwindow.ui \
    menu/markermenu.ui



DISTFILES += \
    resources/textures/marker.jpg \
    resources/icons/camera.png \
    resources/icons/marker.png \
    resources/shaders/ads_fragment.frag \
    resources/shaders/depth.frag \
    resources/shaders/markertexture.frag \
    resources/shaders/normals.frag \
    resources/shaders/picking.frag \
    resources/shaders/ads_fragment.vert \
    resources/shaders/depth.vert \
    resources/shaders/markertexture.vert \
    resources/shaders/normals.vert \
    resources/shaders/picking.vert
RESOURCES += \
    resources.qrc
unix: !macx {
    INCLUDEPATH +=  /usr/include
    LIBS += /home/al/Downloads/assimp-3.1.1/lib/libassimp.so
}

macx {
    INCLUDEPATH +=  /usr/local/include
    LIBS += /usr/local/lib/libassimp.dylib
}

win32 {
    INCLUDEPATH += "C:/Assimp3/include"
    LIBS += -L"C:/Assimp3/lib/Release" -lassimp
}
