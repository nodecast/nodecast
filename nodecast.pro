#-------------------------------------------------
#
# Project created by QtCreator 2014-04-29T15:02:37
#
#-------------------------------------------------

QT       += core gui network opengl webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nodecast
TEMPLATE = app

#INCLUDEPATH += libs/qmpwidget/src
#QMAKE_LIBDIR += libs/qmpwidget

#LIBS += -lqmpwidget
#LIBS += -L"./libs/qmpwidget/" libqmpwidget.a

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10

OBJECTIVE_SOURCES += osxutil.mm


include(libs/qtlibtorrent/qtlibtorrent.pri)
include(libs/tracker/tracker.pri)
include(libs/geoip/geoip.pri)
include(preferences/preferences.pri)
# VERSION DEFINES
include(version.pri)


# Use pkg-config to get all necessary libtorrent DEFINES
CONFIG += link_pkgconfig
CONFIG += c++11
CONFIG += debug

#PKGCONFIG += libtorrent-rasterbar

# LIBTORRENT DEFINES
#DEFINES += WITH_SHIPPED_GEOIP_H
DEFINES += BOOST_ASIO_DYN_LINK
#DEFINES += BOOST_ASIO_SEPARATE_COMPILATION
#DEFINES += WITH_GEOIP_EMBEDDED

INCLUDEPATH += /usr/local/include/
LIBS += -L/usr/local/lib


# Boost system lib
#LIBS += -lboost_system-mt
#LIBS += -ltorrent-rasterbar
#LIBS += /usr/local/lib/libboost_system-mt.a
#LIBS += /usr/local/lib/libtorrent-rasterbar.a


LIBS += -ltorrent-rasterbar \
        -lboost_system \
        -lssl -lcrypto -lidn -lpthread -lz


# Boost filesystem lib (Not needed for libtorrent >= 0.16.0)
#LIBS += -lboost_filesystem-mt
# Carbon
mac: LIBS += -framework Cocoa -framework IOKit


OBJECTIVE_SOURCES += osxutil.mm


#LIBS += /usr/local/Cellar/libtorrent-rasterbar/1.0.2_1/lib/libtorrent-rasterbar.a
#LIBS += -lboost_system-mt

SOURCES += main.cpp\
        mainwindow.cpp \
    video.cpp \
    torrent.cpp \
    fs_utils.cpp \
    misc.cpp \
    downloadthread.cpp \
    scannedfoldersmodel.cpp \
    iconprovider.cpp \
    openuritorrent.cpp \
    widgettorrent.cpp \
    transferlistwidget.cpp \
    statussortfilterproxymodel.cpp \
    flowlayout.cpp \
    account.cpp \
    godcast_api.cpp \
    newsphere.cpp \
    sphere.cpp

HEADERS  += mainwindow.h \
    video.h \
    torrent.h \
    filesystemwatcher.h \
    fs_utils.h \
    misc.h \
    downloadthread.h \
    scannedfoldersmodel.h \
    iconprovider.h \
    qinisettings.h \
    openuritorrent.h \
    downloadfromurldlg.h \
    widgettorrent.h \
    transferlistdelegate.h \
    transferlistfilterswidget.h \
    transferlistsortmodel.h \
    transferlistwidget.h \
    torrentfilterenum.h \
    statussortfilterproxymodel.h \
    torrentpersistentdata.h \
    flowlayout.h \
    about_imp.h \
    account.h \
    godcast_api.h \
    newsphere.h \
    sphere.h

FORMS    += mainwindow.ui \
    video.ui \
    openuritorrent.ui \
    downloadfromurldlg.ui \
    widgettorrent.ui \
    about.ui \
    account.ui \
    newsphere.ui

RESOURCES += \
    nodecast.qrc \
    icons.qrc \
    lang.qrc \
    about.qrc \
    stylesheet.qrc


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/qmpwidget/release/ -lqmpwidget
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/qmpwidget/debug/ -lqmpwidget
#else:unix: LIBS += -L$$PWD/libs/qmpwidget/ -lqmpwidget

#INCLUDEPATH += $$PWD/libs/qmpwidget
#DEPENDPATH += $$PWD/libs/qmpwidget

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/qmpwidget/release/libqmpwidget.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/qmpwidget/debug/libqmpwidget.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libs/qmpwidget/release/qmpwidget.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libs/qmpwidget/debug/qmpwidget.lib
#else:unix: PRE_TARGETDEPS += $$PWD/libs/qmpwidget/libqmpwidget.a

#macx: LIBS += -L$$PWD/../../../../../../usr/local/lib/ -ltorrent-rasterbar

#INCLUDEPATH += $$PWD/../../../../../../usr/local/lib
#DEPENDPATH += $$PWD/../../../../../../usr/local/lib

#macx: PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/libtorrent-rasterbar.a
