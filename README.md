nodecast
========

Nodecast is a file sharing software using bittorrent protocol and based on qBitorrent code.
It aims to be really simple by using spheres and respectful of private life.

Ubuntu :
========

apt-get install libqt5webkit5-dev libtorrent-rasterbar-dev libboost-all-dev libidn11-dev 

git submodule update --init

cd src/libs/qxmpp/

qmake QXMPP_LIBRARY_TYPE=staticlib && make

Mac OSX :
========

brew install boost openssl libtorrent-rasterbar

git submodule update --init

cd src/libs/qxmpp/

qmake QXMPP_LIBRARY_TYPE=staticlib && make
