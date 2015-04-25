nodecast
========

Nodecast is a file sharing software using bittorrent protocol and based on qBitorrent code.
It aims to be really simple by using spheres and respectful of private life.

Ubuntu :
========

apt-get install libqt5webkit5-dev libtorrent-rasterbar-dev libboost-all-dev libidn11-dev libqxmpp-dev

git submodule update --init

cd src/libs/qxmpp/
qmake && make
sudo make install

Mac OSX :
========

brew install boost openssl qxmpp libtorrent-rasterbar

Test :

File open, and paste this link http://www.freetorrent.fr/download.php?id=a751f302a3bd84b5a009a580dff4edebf559127e&f=chiffrofete-up-14.10.torrent

