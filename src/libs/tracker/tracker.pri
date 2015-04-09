INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/qtracker.h \
    $$PWD/trackerannouncerequest.h \
    $$PWD/qpeer.h \
    $$PWD/qtrackerserv.h \
    libs/tracker/http/connection.h \
    libs/tracker/http/irequesthandler.h \
    libs/tracker/http/requestparser.h \
    libs/tracker/http/responsebuilder.h \
    libs/tracker/http/responsegenerator.h \
    libs/tracker/http/server.h \
    libs/tracker/http/types.h

SOURCES += \
    $$PWD/qtracker.cpp \
    $$PWD/qtrackerserv.cpp \
    libs/tracker/http/connection.cpp \
    libs/tracker/http/requestparser.cpp \
    libs/tracker/http/responsebuilder.cpp \
    libs/tracker/http/responsegenerator.cpp \
    libs/tracker/http/server.cpp
