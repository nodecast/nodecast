/*
 * Bittorrent Client using Qt4 and libtorrent.
 * Copyright (C) 2006  Christophe Dumez
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If you
 * modify file(s), you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * Contact : chris@qbittorrent.org
 */

#include <QTcpSocket>
#include <QUrl>

#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>

#include "qtracker.h"
#include "preferences.h"

using namespace libtorrent;

//QTracker::QTracker(QObject *parent) :
//  QTcpServer(parent)
//{
//  Q_ASSERT(Preferences().isTrackerEnabled());
//  connect(this, SIGNAL(newConnection()), this, SLOT(handlePeerConnection()));
//}






//void QTracker::handlePeerConnection()
//{
//  QTcpSocket *socket;
//  while((socket = nextPendingConnection()))
//  {
//    qDebug("QTracker: New peer connection");
//    connect(socket, SIGNAL(readyRead()), SLOT(readRequest()));
//  }
//}

//bool QTracker::start()
//{
//  const int listen_port = Preferences().getTrackerPort();
//  //
//  if (isListening()) {
//    if (serverPort() == listen_port) {
//      // Already listening on the right port, just return
//      return true;
//    }
//    // Wrong port, closing the server
//    close();
//  }
//  qDebug("Starting the embedded tracker...");
//  // Listen on the predefined port
//  return listen(QHostAddress::Any, listen_port);
//}









QTracker::QTracker(int socketDescriptor, QObject *parent)
    : socketDescriptor(socketDescriptor)
{

}



QTracker::~QTracker() {
//  if (isListening()) {
//    qDebug("Shutting down the embedded tracker...");
//    close();
//  }
//  // TODO: Store the torrent list


    if (tcpSocket->state() != QTcpSocket::UnconnectedState)
        tcpSocket->waitForDisconnected(-1);

    //if (tcpSocket->isOpen()) tcpSocket->close();
    tcpSocket->deleteLater();
    qDebug() << "QTracker destructor";
}



void QTracker::init()
{
    qDebug() << "void QTracker::init()";


    tcpSocket = new QTcpSocket(this);
    if (!tcpSocket->setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket->error());
        return;
    }
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readRequest()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(discardClient()));
}


void QTracker::discardClient()
{
    qDebug() << "DISCARD CLIENT";
    this->deleteLater();

    //QtServiceBase::instance()->logMessage("Connection closed");
}




void QTracker::readRequest()
{
    if (tcpSocket->canReadLine())
    {
        QStringList tokens = QString(tcpSocket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
        qDebug() << "METHOD : " << tokens[0];


        //QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
        QString input = tcpSocket->readAll();
        //qDebug("QTracker: Raw request:\n%s", input.data());
        //QHttpRequestHeader http_request(input);



        qDebug() << "INPUT DATAS : " << input;

        QStringList raw_datas = input.split("\r\n");
        qDebug() << "RAW DATAS : " << raw_datas[0];

        QString headers;
        QStringList header;
        foreach (headers, raw_datas)
        {
            //qDebug() << "CONTENAIR : " << headers;
            header = headers.split(":");
            // if (header[0] == "Range") break;
        }
        qDebug() << "HEADER : " << header;




//        if (!http_request.isValid()) {
//        qDebug("QTracker: Invalid HTTP Request:\n %s", qPrintable(http_request.toString()));
//        respondInvalidRequest(socket, 100, "Invalid request type");
//        return;
//        }
        //qDebug("QTracker received the following request:\n%s", qPrintable(parser.toString()));
        // Request is correct, is it a GET request?

        if (tokens[0] != "GET") {
            qDebug() << "nodecast Tracker: Unsupported HTTP request: " << tokens[0];
            respondInvalidRequest(100, "Invalid request type");
            return;
        }

//        if (!http_request.path().startsWith("/announce", Qt::CaseInsensitive)) {
//        qDebug("QTracker: Unrecognized path: %s", qPrintable(http_request.path()));
//        respondInvalidRequest(socket, 100, "Invalid request type");
//        return;
//        }

        // OK, this is a GET request
        // Parse GET parameters

        QHash<QString, QString> get_parameters;
        QUrlQuery url(QUrl::fromEncoded(tokens[1].toLatin1()));

        QListIterator<QPair<QString, QString> > i(url.queryItems());
        while (i.hasNext())
        {
            QPair<QString, QString> pair = i.next();
            get_parameters[pair.first] = pair.second;
        }

        respondToAnnounceRequest(get_parameters);
    }
}

void QTracker::respondInvalidRequest(int code, QString msg)
{
    //  QHttpResponseHeader response;
    //  response.setStatusLine(code, msg);
    QString response = QString::number(code) + " " + msg;
    tcpSocket->write(response.toLocal8Bit());
    tcpSocket->disconnectFromHost();
}





//Do some input validation
//QString QTracker::getkey(QUrl url, QString key, bool &error, bool fixed_size) {
//    QString val = url.queryItemValue(key);
//    QString msg;

//    if (!error && val.isEmpty ())
//    {
//        error = true;
//        msg = "Invalid request, missing data";
//        msg.prepend("d14:failure reason msg.size():");
//        msg.append("e");
//        return msg;

//    }
//    else if (!error && fixed_size && val.size () != 20)
//    {
//        error = true;
//        msg = "Invalid request, length on fixed argument not correct";
//        msg.prepend("d14:failure reason msg.size():");
//        msg.append("e");
//        return msg;
//    }
//    else if (!error && val.size () > 80)
//    { //128 chars should really be enough
//        error = true;
//        msg = "Request too long";
//        msg.prepend("d14:failure reason msg.size():");
//        msg.append("e");
//        return msg;
//    }

//    if (val.isEmpty() && (key == "downloaded" || key == "uploaded" || key == "left"))
//            val="0";

//    return val;
//}



void QTracker::respondToAnnounceRequest(const QHash<QString, QString>& get_parameters)
{
    TrackerAnnounceRequest annonce_req;
    // IP
    annonce_req.peer.ip = tcpSocket->peerAddress().toString();
    // 1. Get info_hash
    if (!get_parameters.contains("info_hash"))
    {
        qDebug("nodetracker: Missing info_hash");
        respondInvalidRequest(101, "Missing info_hash");
        return;
    }
    annonce_req.info_hash = get_parameters.value("info_hash");
    // info_hash cannot be longer than 20 bytes
    /*if (annonce_req.info_hash.toAscii().length() > 20) {
    qDebug("QTracker: Info_hash is not 20 byte long: %s (%d)", qPrintable(annonce_req.info_hash), annonce_req.info_hash.toAscii().length());
    respondInvalidRequest(socket, 150, "Invalid infohash");
    return;
    }*/
    // 2. Get peer ID
    if (!get_parameters.contains("peer_id"))
    {
        qDebug("nodetracker: Missing peer_id");
        respondInvalidRequest(102, "Missing peer_id");
        return;
    }
    annonce_req.peer.peer_id = get_parameters.value("peer_id");
    // peer_id cannot be longer than 20 bytes
    /*if (annonce_req.peer.peer_id.length() > 20) {
    qDebug("QTracker: peer_id is not 20 byte long: %s", qPrintable(annonce_req.peer.peer_id));
    respondInvalidRequest(socket, 151, "Invalid peerid");
    return;
    }*/
    // 3. Get port
    if (!get_parameters.contains("port"))
    {
        qDebug("nodetracker: Missing port");
        respondInvalidRequest(103, "Missing port");
        return;
    }
    bool ok = false;
    annonce_req.peer.port = get_parameters.value("port").toInt(&ok);
    if (!ok || annonce_req.peer.port < 1 || annonce_req.peer.port > 65535)
    {
        qDebug("nodetracker: Invalid port number (%d)", annonce_req.peer.port);
        respondInvalidRequest(103, "Missing port");
        return;
    }
    // 4.  Get event
    annonce_req.event = "";
    if (get_parameters.contains("event"))
    {
        annonce_req.event = get_parameters.value("event");
        qDebug("nodetracker: event is %s", qPrintable(annonce_req.event));
    }
    // 5. Get numwant
    annonce_req.numwant = 50;
    if (get_parameters.contains("numwant"))
    {
        int tmp = get_parameters.value("numwant").toInt();
        if (tmp > 0)
        {
            qDebug("nodetracker: numwant=%d", tmp);
            annonce_req.numwant = tmp;
        }
    }
    // 6. no_peer_id (extension)
    annonce_req.no_peer_id = false;
    if (get_parameters.contains("no_peer_id"))
    {
        annonce_req.no_peer_id = true;
    }
    // 7. TODO: support "compact" extension
    // Done parsing, now let's reply
    if (m_torrents.contains(annonce_req.info_hash))
    {
        if (annonce_req.event == "stopped")
        {
            qDebug("nodetracker: Peer stopped downloading, deleting it from the list");
            m_torrents[annonce_req.info_hash].remove(annonce_req.peer.qhash());
            return;
    }
    } else {
    // Unknown torrent
    if (m_torrents.size() == MAX_TORRENTS)
    {
      // Reached max size, remove a random torrent
      m_torrents.erase(m_torrents.begin());
    }
    }
    // Register the user
    PeerList peers = m_torrents.value(annonce_req.info_hash);
    if (peers.size() == MAX_PEERS_PER_TORRENT)
    {
        // Too many peers, remove a random one
        peers.erase(peers.begin());
    }
    peers[annonce_req.peer.qhash()] = annonce_req.peer;
    m_torrents[annonce_req.info_hash] = peers;
    // Reply
    ReplyWithPeerList(annonce_req);
}

void QTracker::ReplyWithPeerList(const TrackerAnnounceRequest &annonce_req)
{  
    // Prepare the entry for bencoding
    entry::dictionary_type reply_dict;
    reply_dict["interval"] = entry(ANNOUNCE_INTERVAL);
    QList<QPeer> peers = m_torrents.value(annonce_req.info_hash).values();
    entry::list_type peer_list;
    foreach (const QPeer & p, peers)
    {
        //if (p != annonce_req.peer)
        peer_list.push_back(p.toEntry(annonce_req.no_peer_id));
    }
    reply_dict["peers"] = entry(peer_list);
    entry reply_entry(reply_dict);
    // bencode
    std::vector<char> buf;
    bencode(std::back_inserter(buf), reply_entry);
    QByteArray reply(&buf[0], buf.size());
    qDebug("QTracker: reply with the following bencoded data:\n %s", reply.constData());
    // HTTP reply
    //QHttpResponseHeader response;
    //response.setStatusLine(200, "OK");
    QString response = "200 OK \r\n";
    tcpSocket->write(response.toLocal8Bit() + reply);
    tcpSocket->disconnectFromHost();
}


