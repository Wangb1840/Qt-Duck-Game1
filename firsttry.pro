QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

CONFIG += c++11

TARGET = jiangbanya_run
TEMPLATE = app

SOURCES += \
    main.cpp \
    gamewidget.cpp \
    player.cpp \
    obstacle.cpp

HEADERS += \
    gamewidget.h \
    player.h \
    obstacle.h

RESOURCES += \
    resources.qrc \
    resources.qrc

DISTFILES += \
    images/baihu.jpg.jpg
