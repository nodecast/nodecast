#include "torrent.h"


Torrent *Torrent::sPtr = 0;



tAux::tAux( libtorrent::torrent_handle hndl, int index, long long size )
   : handle( hndl ), fileIndex( index ), fileSize( size )
{
    handle.set_sequential_download( false );
    libtorrent::torrent_info ti = handle.get_torrent_info();
    fileFirstPiece = ti.map_file( fileIndex, 0, 1 ).piece;
    fileLastPiece = ti.map_file( fileIndex, fileSize - 1, 1 ).piece;
    handle.piece_priority( fileLastPiece, 7 );
    handle.piece_priority( fileLastPiece - 1, 7 );
    pieceLength = ti.piece_length();
    for ( int i = fileFirstPiece; i < fileFirstPiece + MAX_PRIORITY_PIECES && i < fileLastPiece; ++i ) handle.piece_priority( i, 7 );
}

void tAux::prioritize()
{
    if ( !handle.is_valid() ) return;
    if ( !handle.have_piece( fileLastPiece ) ) return;
//  USE_DEBUG( "Prioritize\n" );
    int piecesToSet = MAX_PRIORITY_PIECES;
    int latestPiece = fileLastPiece;
//    bool hadPiece = true;
    for ( int i = fileFirstPiece; i < fileLastPiece && i < latestPiece && piecesToSet; ++i )
    {
        if ( handle.have_piece( i ) ) continue;
        else if ( latestPiece >= fileLastPiece ) latestPiece = i + MAX_PRIORITY_PIECES;
        if ( handle.piece_priority( i ) != 7 )
        {
            handle.piece_priority( i, 7 );
            qDebug() << "Prioritize piece " << i ;
        }
        --piecesToSet;
    }
}

bool tAux::canGetUrl()
{
    if ( !handle.is_valid() ) return false;
    libtorrent::bitfield pieces = handle.status( libtorrent::torrent_handle::query_pieces ).pieces;
    if ( !pieces.get_bit( fileLastPiece ) ) return false;
    int piecesToSet = 0;
//  handle.set_sequential_download( true );
    for ( int i = fileFirstPiece; i < fileLastPiece; ++i )
    {
        if ( !handle.have_piece( i ) ) return false;
        if ( ++piecesToSet >= MIN_START_PIECES ) return true;
    }
    return true;
}









Torrent::Torrent(QObject *parent) :
    QObject(parent), nextId( 0 ),
    torrent_session(libtorrent::fingerprint("nodecast", LIBTORRENT_VERSION_MAJOR, LIBTORRENT_VERSION_MINOR, 0, 0)
                    , libtorrent::session::add_default_plugins
                    , libtorrent::alert::all_categories
                    & ~(libtorrent::alert::dht_notification
                        + libtorrent::alert::progress_notification
                        + libtorrent::alert::debug_notification
                        + libtorrent::alert::stats_notification))

/*
torrent_session( libtorrent::fingerprint( "nodecast", LIBTORRENT_VERSION_MAJOR, LIBTORRENT_VERSION_MINOR, 0, 0 )
                 , libtorrent::session::add_default_plugins
                 , libtorrent::alert::progress_notification )
*/

{

    qDebug() << "TORRENT instancied";

    active_torrent = 0;
}


void Torrent::init()
{
  /*  torrent_session.listen_on(std::make_pair(6881, 6889), torrent_error_code);
    if (torrent_error_code)
    {
      qDebug() << "failed to open listen socket: " << torrent_error_code.message().c_str();
      //return 1;
    }
    */


    torrent_session.add_extension(&libtorrent::create_ut_pex_plugin);
    torrent_session.add_extension(&libtorrent::create_smart_ban_plugin);

    torrent_session.start_lsd();
    torrent_session.start_upnp();
    torrent_session.start_natpmp();

    libtorrent::session_settings sets = torrent_session.settings();


    sets.announce_to_all_trackers = true;
    sets.announce_to_all_tiers = true;
    sets.prefer_udp_trackers = true;
    sets.max_peerlist_size = 0;
    //sets.announce_ip = true;
    sets.user_agent = "nodecast";
    sets.active_seeds = true;

    torrent_session.set_settings( sets );


}

Torrent::~Torrent()
{
    qDebug() << "END TORRENT";
  //  torrent_file->close();
    sleep( 2 );

    qDebug() << "TORRENT CLOSED";
}


bool Torrent::get_torrent(QString torrent)
{

    torrent_file = new QFile(torrent);


    if (!torrent_file->open(QIODevice::ReadOnly))
           return 1;

    torrent_param.ti = new libtorrent::torrent_info(torrent_file->fileName().toStdString(), torrent_error_code);
    if (torrent_error_code)
    {
        qDebug() << torrent_error_code.message().c_str();
        return 1;
    }
    torrent_session.add_torrent(torrent_param, torrent_error_code);
    if (torrent_error_code)
    {
        qDebug() << torrent_error_code.message().c_str();
        return 1;
    }

    qDebug() << "SESSION STATUS : " << torrent_session.status().num_peers;


    //libtorrent::torrent_status const& st = get_active_torrent(filtered_handles);


      // wait for the user to end
    //  char a;
    //  scanf("%c\n", &a);
    return 0;
}




int Torrent::downloadTorrent( QString url, QString fileName )
{
    if ( url.isEmpty() ) return -8;
    libtorrent::add_torrent_params p;
    p.url = url.toStdString();
    return addTorrent( p, fileName);
}





int Torrent::addTorrent( libtorrent::add_torrent_params& p, QString fileName )
{
    libtorrent::torrent_handle h;

    //p.save_path = tempDirValid() ? getTempDir() : std::string( "." );
    p.save_path = settings.value("data").toString().toStdString();
    p.storage_mode = libtorrent::storage_mode_allocate;
    int index = getNextId();

    qDebug() << "INDEX : " << index;

    libtorrent::error_code ec;
    h = torrent_session.add_torrent( p, ec );
    if ( ec )
    {
        qDebug() << "Torrent add error: " << ec.message().c_str();
        return -10;
    }

    time_t timeout = time( NULL ) + 30;
   // boost::chrono::milliseconds dura( 500 );
    //while ( time( NULL ) < timeout && h.is_valid() && !h.status( 0 ).has_metadata ) boost::this_thread::sleep_for( dura );

    while ( time( NULL ) < timeout && h.is_valid() && !h.status( 0 ).has_metadata ) this->thread()->wait( 500 );

    if ( !h.is_valid() ) return -10;
    if ( !h.status( 0 ).has_metadata )
    {
        qDebug() << "Torrent metadata download timeout";
        torrent_session.remove_torrent( h );
        return -10;
    }

    int longestIndex = -1;
    size_t longestSize = 0;

    try
    {
        longestIndex = getFileIndex( h, fileName.toStdString() );
        if ( longestIndex < 0 )
        {
            qDebug() << "no file selected";
            deleteTorrent( index );
            return -9;
        }
        longestSize = h.get_torrent_info().file_at( longestIndex ).size;
    }
    catch (...)
    {
        qDebug() << "Exception on getFileIndex";
        deleteTorrent( index );
        return -9;
    }

    qDebug() << "torrent added.";
    tMap.insert(index, tAux( h, longestIndex, longestSize ) );
    return index;
}



bool Torrent::deleteTorrent( int torrentId, bool deleteCompleted )
{
    torrentMap::iterator it = tMap.find( torrentId );
    if ( it == tMap.end() )
    {
        qDebug() << "Can't find data for torrent index : " << torrentId;
        return false;
    }
    libtorrent::torrent_handle *t = &it.value().handle;
    const bool hadTorrent = t && t->is_valid();
    if ( hadTorrent )
    {
        try
        {
            torrent_session.remove_torrent( *t, deleteCompleted ? libtorrent::session::delete_files : 0 );
        }
        catch (...)
        {
            qDebug() << "Torrent removal error, index : " << torrentId;
        }
    }
    else qDebug() << "Cant't associate torrent index : " << torrentId;
    tMap.erase( it );
    qDebug() << "Torrent " << torrentId << " removed";
    return hadTorrent;
}



int Torrent::getFileIndex( libtorrent::torrent_handle& h, std::string fileName )
{
    if ( !h.is_valid() || !h.status( 0 ).has_metadata ) return -1;
    libtorrent::torrent_info ti = h.get_torrent_info();
    if ( !ti.is_valid() ) return -1;

    std::vector<boost::uint8_t> filePrio;
    size_t longestSize = 0;
    int longestIndex = -1;

    for ( int i = 0; i < ti.num_files(); ++i )
    {
        h.file_priority( i, 0 );
        const libtorrent::file_entry file = ti.file_at( i );
#ifdef WIN32
        qDebug( "[%i] %08I64d %s\n", i, file.size, file.path.c_str() );
#else
        qDebug( "[%i] %08lld %s\n", i, file.size, file.path.c_str() );
#endif
        if ( file.pad_file || longestSize == std::numeric_limits<int64_t>::max() ) continue;
        if ( file.size > longestSize )
        {
            longestSize = file.size;
            longestIndex = i;
        }
        if ( fileName.size() && file.path.find( fileName ) != std::string::npos )
        {
            longestIndex = i;
            longestSize = std::numeric_limits<int64_t>::max();
        }
    }
    if ( longestIndex >= ti.num_files() ) return -1;
//  filePrio[longestIndex] = 1;
#ifdef WIN32
    qDebug( "selected [%i] %08I64d %s\n", longestIndex, ti.file_at( longestIndex ).size, ti.file_at( longestIndex ).path.c_str() );
#else
    qDebug( "selected [%i] %08lld %s\n", longestIndex, ti.file_at( longestIndex ).size, ti.file_at( longestIndex ).path.c_str() );
#endif
    return longestIndex;
}





/*

libtorrent::torrent_status const& Torrent::get_active_torrent(std::vector<libtorrent::torrent_status const*> const& filtered_handles)
{
    if (active_torrent >= int(filtered_handles.size())
        || active_torrent < 0) active_torrent = 0;
    return *filtered_handles[active_torrent];
}
*/




tAux* Torrent::getAux( int torrentId )
{
    torrentMap::iterator it = tMap.find( torrentId );
    if ( it == tMap.end() ) return 0;
   // tAux *t = &it->second;
    tAux *t = &it.value();

    if ( t && t->handle.is_valid() ) return t;
    tMap.erase( it );
    return 0;
}


dlInfo Torrent::getTorrentInfo( int torrentId )
{
    dlInfoExt result;

    tAux *tInf = getAux( torrentId );
    if ( !tInf ) return result;

    result.total = tInf->fileSize;

    libtorrent::torrent_handle *t = &tInf->handle;

//  torrent_status ts = t->status( 0 );
    libtorrent::torrent_status ts = t->status( libtorrent::torrent_handle::query_pieces );
    result.downloadRateBs = ts.download_payload_rate;
    result.seeders = ts.num_seeds;
    result.peers = ts.num_peers;
    result.piecesDone = ts.num_pieces;
    libtorrent::torrent_info ti = t->get_torrent_info();
    result.piecesTotal = ti.num_pieces(); ///@todo select pieces from file!!!
    result.pieceSize = ti.piece_length();
    result.downloaded = tInf->fileSize == ts.total_wanted_done ? tInf->fileSize : countPieces( ts.pieces, tInf->fileFirstPiece, tInf->fileLastPiece ) * ti.piece_length();
//  result.downloaded = ts.total_wanted_done;

    const int first = tInf->fileFirstPiece;
    const int last = tInf->fileLastPiece;
    qDebug( "File pieces %i..%i piece1:%i, piece2:%i, piece3:%i, piece4:%i, piece5:%i, last piece:%i\n",
               first, last,
               ts.pieces.get_bit( first ),
               ts.pieces.get_bit( first + 1 ),
               ts.pieces.get_bit( first + 2 ),
               ts.pieces.get_bit( first + 3 ),
               ts.pieces.get_bit( first + 4 ),
               ts.pieces.get_bit( last )
              );

//  std::string temp( "Active pieces: " );
//  for ( int i = first; i < last; ++i )
//  {
//      if ( ts.pieces.get_bit( i ) ) temp += boost::lexical_cast<std::string>( i ) + ",";
//  }
//  USE_DEBUG( temp.c_str() );
    return result;
}




int Torrent::countPieces( libtorrent::bitfield& pieces, int first, int last )
{
    int piecesDone = 0;
    for ( int i = first; i <= last; ++i ) if ( pieces.get_bit( i ) ) ++piecesDone;
        else break;
    return piecesDone;
}


