#-------------------------------------------------
#
# Project created by QtCreator 2018-01-05T09:32:16
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt45MemoryManager_V03
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dbmanager.cpp \
    exif.cpp

HEADERS  += mainwindow.h \
    dbmanager.h \
    exif.h

FORMS    += mainwindow.ui
