nodecast
========

Nodecast is a file sharing software using bittorrent protocol and based on qBitorrent code.
It aims to be really simple by using spheres and respectful of private life.

Ubuntu :
========

apt-get install libqt5webkit5-dev libtorrent-rasterbar-dev libboost-all-dev libidn11-dev 


Mac OSX :
========

brew install boost openssl libtorrent-rasterbar qt5 pkg-config

COMPILE :
========

git submodule update --init

cd src/libs/qxmpp/

qmake && make

make install

cd ../../../

qmake && make


USE
====

You have to use an xmpp account with file transfert capability and open to Server2Server (not gmail account). You can use your own jabber server like prosody.im.
