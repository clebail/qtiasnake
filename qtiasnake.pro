QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    couche.cpp \
    game.cpp \
    main.cpp \
    mainwindow.cpp \
    neurone.cpp \
    random.cpp \
    reseau.cpp \
    reseauwidget.cpp \
    sensor.cpp \
    snakewidget.cpp

HEADERS += \
    couche.h \
    game.h \
    mainwindow.h \
    neurone.h \
    random.h \
    reseau.h \
    reseauwidget.h \
    sensor.h \
    snakewidget.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
