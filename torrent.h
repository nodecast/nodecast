#ifndef TORRENT_H
#define TORRENT_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QHash>
#include <QThread>

#include <stdlib.h>
#include <libtorrent/entry.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/storage.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/file.hpp>
#include <libtorrent/hasher.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/file.hpp>
#include <libtorrent/file_pool.hpp>
#include <libtorrent/extensions/ut_pex.hpp>
#include <libtorrent/extensions/logger.hpp>
#include <libtorrent/extensions/smart_ban.hpp>

#include <boost/bind.hpp>


const int MAX_PRIORITY_PIECES = 5;
const int MIN_START_PIECES = 5;

const int TORRENT_INFO_UPDATES_PER_SECOND = 3;
const int TORRENT_PIECES_TO_PLAY = 7;


struct tAux
{
    tAux( libtorrent::torrent_handle hndl, int index, long long size );
    void prioritize();
    bool canGetUrl();
    libtorrent::torrent_handle handle;
    int fileIndex;
    long long fileSize;
    int pieceLength;
    int fileFirstPiece;
    int fileLastPiece;
    std::string fileUri;
    std::string filePath;
};


struct dlInfo
{
    long long downloadRateBs;
    long long downloaded;
    long long total;
    long long piecesDone;
    long long piecesTotal;
    long long pieceSize;
    long long seeders;
    long long peers;
};



struct dlInfoExt : public dlInfo
{
    dlInfoExt()
    {
        downloadRateBs = 0;
        downloaded = 0;
        total = 0;
        piecesDone = 0;
        piecesTotal = 0;
        pieceSize = 0;
        seeders = 0;
        peers = 0;
    }
};

struct NetUsage
{
    long long uploadRateBs;
    long long downloadRateBs;
    long long uploaded;
    long long downloaded;
};





typedef QMap<int, tAux> torrentMap;


class Torrent : public QObject
{
    Q_OBJECT
public:
    explicit Torrent(QObject *parent = 0);
    ~Torrent();
    int downloadTorrent( QString url, QString fileName = 0 );
    bool get_torrent(QString torrent);
    dlInfo getTorrentInfo( int torrentId );
    virtual bool deleteTorrent( int torrentId, bool deleteCompleted = true );

    tAux* getAux( int torrentId );
    int getNextId()
      {
          if ( nextId + 1 == INT_MAX ) nextId = 0;
          return ++nextId;
      }

    static Torrent *sPtr;
    static int countPieces( libtorrent::bitfield& pieces, int first, int last );


signals:

public slots:
    void init();

private:
    int addTorrent( libtorrent::add_torrent_params& p, QString fileName );
    int getFileIndex( libtorrent::torrent_handle& h, std::string fileName = std::string() );


    int nextId;
    QFile *torrent_file;
    QSettings settings;
    libtorrent::session torrent_session;
    libtorrent::error_code torrent_error_code;
    libtorrent::add_torrent_params torrent_param;
    int active_torrent;
    std::vector<libtorrent::torrent_status const*> filtered_handles;
    torrentMap tMap;


   // libtorrent::torrent_status const& get_active_torrent(std::vector<libtorrent::torrent_status const*> const& filtered_handles);
};

#endif // TORRENT_H


