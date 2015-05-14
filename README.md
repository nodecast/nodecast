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


NEED TO KNOW
============


When you drag a file or a directory in a sphere, nodecast create a symlink to the source before create the torrent file. So you can drag a big file or directory without fear, nodecast will not need to make a local copy.

When you delete a media shared from you, if you make choice to delete files from your hard disk, nodecast delete only the symlink and not to real file or directory.

When you delete a media received from a contact, if you make choice to delete files from your hard disk, nodecast delete the file or the directory. So if you need to keep the media, right clic on it, select open destination folder, and make a backup before delete him.
