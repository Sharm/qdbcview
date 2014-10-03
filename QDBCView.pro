#-------------------------------------------------
#
# Project created by QtCreator 2010-07-18T11:50:54
#
#-------------------------------------------------

QT       += core gui

TARGET = QDBCView
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qdbctablemodel.cpp \
    qdbcfile.cpp \
    qdbcinstance.cpp

HEADERS  += mainwindow.h \
    ui_mainwindow.h \
    qdbctablemodel.h \
    constants.h \
    qdbcfile.h \
    qdbcinstance.h

FORMS    += \
    dockwidget.ui

RESOURCES += \
    dbc.qrc

win32:RC_FILE += dbc.rc
