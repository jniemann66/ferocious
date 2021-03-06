#-------------------------------------------------
#
# Project created by QtCreator 2016-04-02T14:51:56
#
#-------------------------------------------------

# Application version:
VERSION = 2.1.3

# Define a preprocessor macro for the version:
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT += core gui concurrent widgets
CONFIG += c++11
TARGET = ferocious
TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp \
    outputfileoptions_dialog.cpp \
    fancylineedit.cpp \
    lpfparametersdlg.cpp \
    converteroutputbox.cpp \
    filedropdialog.cpp \
    filedroptextedit.cpp \
    converterdefinition.cpp \
    converterconfigurationdialog.cpp \
    converterconfigurationeditdialog.cpp \
    cmdlinehighlighterdelegate.cpp \
    checkboxdelegate.cpp \
    converterdefinitionsmodel.cpp

HEADERS  += mainwindow.h \
    flashingpushbutton.h \
    outputfileoptions_dialog.h \
    fancylineedit.h \
    lpfparametersdlg.h \
    converteroutputbox.h \
    filedropdialog.h \
    filedroptextedit.h \
    converterdefinition.h \
    converterconfigurationdialog.h \
    converterconfigurationeditdialog.h \
    cmdlinehighlighterdelegate.h \
    checkboxdelegate.h \
    converterdefinitionsmodel.h

FORMS    += mainwindow.ui \
    outputfileoptions_dialog.ui \
    lpfparametersdlg.ui

RC_FILE = ferocious.rc

RESOURCES += \
    ferocious.qrc \
    converterdefinitions.qrc
