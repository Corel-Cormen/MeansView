QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    filter.cpp \
    filterdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    opendatadialog.cpp \
    plot.cpp \
    plotproperties.cpp

HEADERS += \
    filter.h \
    filterdialog.h \
    mainwindow.h \
    opendatadialog.h \
    plot.h \
    plotproperties.h

FORMS += \
    filterdialog.ui \
    mainwindow.ui \
    opendatadialog.ui \
    plotproperties.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

INCLUDEPATH += armadillo/include
LIBS +=  -llapack -lblas
