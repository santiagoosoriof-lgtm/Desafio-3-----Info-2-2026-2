QT += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += resources_big

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Pergamino.cpp \
    capatazmayor.cpp \
    gestornivel1.cpp \
    gestornivel2.cpp \
    indianajones.cpp \
    main.cpp \
    mainwindow.cpp \
    proyectil.cpp \
    remolino.cpp \
    troncomoa.cpp

HEADERS += \
    Corazon.h \
    ObjetoRecolectable.h \
    Obstaculo.h \
    Palanca.h \
    Pergamino.h \
    PuntoAnclaje.h \
    capatazmayor.h \
    entidadjuego.h \
    gestornivel1.h \
    gestornivel2.h \
    indianajones.h \
    mainwindow.h \
    personaje.h \
    proyectil.h \
    remolino.h \
    troncomoa.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += sprites.qrc

RC_ICONS = icono_juego.ico