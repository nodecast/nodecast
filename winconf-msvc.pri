strace_win:{
  contains(QMAKE_HOST.arch, x86):{
    # i686 arch requires frame pointer preservation
    QMAKE_CXXFLAGS_RELEASE += -Oy-
    QMAKE_CXXFLAGS_DEBUG += -Oy-
    QMAKE_CXXFLAGS_DEBUG += -wd4100 -wd4189 -wd4996 -wd4018
  }
  release:{
    QMAKE_CXXFLAGS_RELEASE += -Zi
    QMAKE_LFLAGS += "/DEBUG"
    QMAKE_CXXFLAGS_DEBUG += -wd4100 -wd4189 -wd4996 -wd4018
  }
  LIBS += dbghelp.lib
}

QMAKE_LFLAGS += "/OPT:REF /OPT:ICF"
QMAKE_LFLAGS += "/NODEFAULTLIB:MSVCRT"
QMAKE_LFLAGS += "/LTCG"


# Enable Wide characters
DEFINES += TORRENT_USE_WPATH

#Adapt the lib names/versions accordingly
CONFIG(debug, debug|release) {
  LIBS += torrent.lib \
          boost_system.lib
} else {
  LIBS += torrent.lib \
          boost_system.lib
}

LIBS += advapi32.lib shell32.lib crypt32.lib User32.lib
LIBS += libeay32.lib ssleay32.lib
LIBS += PowrProf.lib
LIBS += zlib.lib
LIBS += qxmpp0.lib
