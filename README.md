nodecast
========

Nodecast is a file sharing software using bittorrent protocol and based on qBitorrent code.
It aims to be really simple by using spheres and respectful of private life.

Ubuntu :
========

apt-get install libqt5webkit5-dev libtorrent-rasterbar-dev libboost-all-dev libidn11-dev 


Mac OSX :
========

install brew http://brew.sh

install Qt5 from http://qt.io

export path to Qt

export PATH=$PATH:/Users/YOURUSER/Qt/5.4/clang_64/bin

brew install boost openssl libtorrent-rasterbar pkg-config

COMPILE :
========

git submodule update --init

cd src/libs/qxmpp/

qmake && make (make -j 4 if you have a quad core)

make install

cd ../../../

qmake && make (make -j 4 if you have a quad core)

Mac OSX :
========

mkdir -p src/nodecast.app/Contents/PlugIns/platforms

cp /Users/youraccount/Qt/5.4/clang_64/plugins/platforms/libqcocoa.dylib src/nodecast.app/Contents/PlugIns/platforms

USE
====

Nodecast / Login to setup your xmpp account

You have to use an xmpp account with file transfert capability and open to Server2Server (not gmail account). You can use your own jabber server like prosody.im.

Nodecast / Preferences to setup bittorrent and active the embedded tracker in the advanced setting
