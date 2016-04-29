#-------------------------------------------------
#
# Project created by QtCreator 2016-04-02T14:51:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ferocious
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    outputfileoptions_dialog.cpp

HEADERS  += mainwindow.h \
    flashingpushbutton.h \
    outputfileoptions_dialog.h

FORMS    += mainwindow.ui \
    outputfileoptions_dialog.ui



RC_FILE = ferocious.rc

RESOURCES += \
    ferocious.qrc


