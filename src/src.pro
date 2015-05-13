# Global
TEMPLATE = app
CONFIG += qt thread
CONFIG += c++11
#DEFINES += QXMPP_STATIC

# Windows specific configuration
win32: include(../winconf.pri)


# Mac specific configuration
macx: include(../macxconf.pri)


# Unix specific configuration
unix:!macx: include(../unixconf.pri)


# eCS(OS/2) specific configuration
os2: include(../os2conf.pri)

include(libs/qxmpp/qxmpp.pri)


QT += xml svg
CONFIG(static) {
  DEFINES += QBT_STATIC_QT
  QTPLUGIN += qico
}
TARGET = nodecast

strace_win: DEFINES += STACKTRACE_WIN
QT += gui network opengl webkitwidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Vars
LIBS_PATH = libs
LANG_PATH = lang
ICONS_PATH = Icons

CONFIG(debug, debug|release):message(Project is built in DEBUG mode.)
CONFIG(release, debug|release):message(Project is built in RELEASE mode.)

# Disable debug output in release mode
CONFIG(release, debug|release) {
   message(Disabling debug output.)
   DEFINES += QT_NO_DEBUG_OUTPUT
}

# VERSION DEFINES
include(../version.pri)

DEFINES += QT_NO_CAST_TO_ASCII
# Fast concatenation (Qt >= 4.6)
DEFINES += QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS

# Fixes compilation with Boost >= v1.46 where boost
# filesystem v3 is the default.
DEFINES += BOOST_FILESYSTEM_VERSION=2

#INCLUDEPATH += $$PWD
win32: DEFINES += NOMINMAX

# Resource files
RESOURCES += \
    nodecast.qrc \
    icons.qrc \
    lang.qrc \
    about.qrc \
    stylesheet.qrc

# Source code
usesystemqtsingleapplication {
    CONFIG += qtsingleapplication
} else {
    include(libs/qtsingleapplication/qtsingleapplication.pri)
}

include(libs/qtlibtorrent/qtlibtorrent.pri)
include(libs/tracker/tracker.pri)
include(libs/geoip/geoip.pri)
include(preferences/preferences.pri)
include(torrentcreator/torrentcreator.pri)
include(xmpp/xmpp.pri)


!nox {
#  include(lineedit/lineedit.pri)
#  include(properties/properties.pri)
#  include(searchengine/searchengine.pri)
#  include(rss/rss.pri)
#  include(torrentcreator/torrentcreator.pri)
#  include(geoip/geoip.pri)
#  include(powermanagement/powermanagement.pri)
}


INCLUDEPATH += $$LIBS_PATH/qxmpp/src/base
INCLUDEPATH += $$LIBS_PATH/qxmpp/src/client
#INCLUDEPATH += $$QXMPP_INCLUDEPATH
LIBS += $$QXMPP_LIBS
#LIBS += $$PWD/$$LIBS_PATH/qxmpp/src/libqxmpp.a

SOURCES +=  main.cpp\
            mainwindow.cpp \
            ico.cpp \
            video.cpp \
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
            sphere.cpp \
    room.cpp \
    roominvit.cpp

HEADERS  += mainwindow.h \
            ico.h \
            video.h \
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
            sphere.h \
    global.h \
    room.h \
    roominvit.h \
    deletionconfirmationdlg.h


win32|macx {
  HEADERS += programupdater.h
  SOURCES += programupdater.cpp
}


FORMS += mainwindow.ui \
    video.ui \
    openuritorrent.ui \
    downloadfromurldlg.ui \
    widgettorrent.ui \
    about.ui \
    account.ui \
    newsphere.ui \
    roominvit.ui \
    confirmdeletiondlg.ui


# OS specific config
OTHER_FILES += ../winconf.pri ../macxconf.pri ../unixconf.pri ../os2conf.pri
# compiler specific config
OTHER_FILES += ../winconf-mingw.pri ../winconf-msvc.pri
# version file
OTHER_FILES += ../version.pri

# Translations
TRANSLATIONS = $$LANG_PATH/qbittorrent_fr.ts \
               $$LANG_PATH/qbittorrent_zh.ts \
               $$LANG_PATH/qbittorrent_zh_TW.ts \
               $$LANG_PATH/qbittorrent_en.ts \
               $$LANG_PATH/qbittorrent_en_AU.ts \
               $$LANG_PATH/qbittorrent_en_GB.ts \
               $$LANG_PATH/qbittorrent_ca.ts \
               $$LANG_PATH/qbittorrent_es.ts \
               $$LANG_PATH/qbittorrent_pl.ts \
               $$LANG_PATH/qbittorrent_ko.ts \
               $$LANG_PATH/qbittorrent_de.ts \
               $$LANG_PATH/qbittorrent_nl.ts \
               $$LANG_PATH/qbittorrent_tr.ts \
               $$LANG_PATH/qbittorrent_sv.ts \
               $$LANG_PATH/qbittorrent_el.ts \
               $$LANG_PATH/qbittorrent_ru.ts \
               $$LANG_PATH/qbittorrent_uk.ts \
               $$LANG_PATH/qbittorrent_bg.ts \
               $$LANG_PATH/qbittorrent_it.ts \
               $$LANG_PATH/qbittorrent_sk.ts \
               $$LANG_PATH/qbittorrent_ro.ts \
               $$LANG_PATH/qbittorrent_pt.ts \
               $$LANG_PATH/qbittorrent_nb.ts \
               $$LANG_PATH/qbittorrent_fi.ts \
               $$LANG_PATH/qbittorrent_da.ts \
               $$LANG_PATH/qbittorrent_ja.ts \
               $$LANG_PATH/qbittorrent_hu.ts \
               $$LANG_PATH/qbittorrent_pt_BR.ts \
               $$LANG_PATH/qbittorrent_cs.ts \
               $$LANG_PATH/qbittorrent_sr.ts \
               $$LANG_PATH/qbittorrent_ar.ts \
               $$LANG_PATH/qbittorrent_hr.ts \
               $$LANG_PATH/qbittorrent_gl.ts \
               $$LANG_PATH/qbittorrent_hy.ts \
               $$LANG_PATH/qbittorrent_lt.ts \
               $$LANG_PATH/qbittorrent_ka.ts \
               $$LANG_PATH/qbittorrent_be.ts \
               $$LANG_PATH/qbittorrent_eu.ts \
               $$LANG_PATH/qbittorrent_he.ts \
               $$LANG_PATH/qbittorrent_vi.ts

DESTDIR = .
