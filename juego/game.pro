QT       += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    arma.cpp \
    caverman.cpp \
    enemigo.cpp \
    main.cpp \
    mainwindow.cpp \
    mamut.cpp \
    modelomovimiento.cpp \
    nivel.cpp \
    nivelmamut.cpp \
    nivelsnowman.cpp \
    nivelvolcan.cpp \
    personaje.cpp \
    proyectil.cpp \
    snowman.cpp \
    sprite.cpp

HEADERS += \
    arma.h \
    caverman.h \
    enemigo.h \
    mainwindow.h \
    mamut.h \
    modelomovimiento.h \
    nivel.h \
    nivelmamut.h \
    nivelsnowman.h \
    nivelvolcan.h \
    personaje.h \
    proyectil.h \
    snowman.h \
    sprite.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    pictures.qrc \
    sonidos.qrc

DISTFILES += \
    escenas/gameover.jpg \
    escenas/menu.avif \
    recursos_juego/bananas.png \
    recursos_juego/corazones.PNG \
    sonidos/Flint - Jungle Pulse - No Lead Vocals.mp3 \
    sonidos/Flint - Jungle Pulse - No Lead Vocals.mp3 \
    sonidos/mamut.wav \
    sprites/lanzas_3.png
RESOURCES += recursos.qrc
QT += multimedia
