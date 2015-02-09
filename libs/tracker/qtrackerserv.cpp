#include "qtrackerserv.h"


/*
Streaming::Streaming(QObject *parent) :
    QObject(parent)
{
}
*/

// HttpDaemon is the the class that implements the simple HTTP server.

Qtrackerserv::Qtrackerserv(quint16 port, QObject *parent)
         : QTcpServer(parent)

{
    //listen(QHostAddress::LocalHost, port);
    listen(QHostAddress::LocalHost, port);

    qDebug() << "READ listen";

 //   t_scf = new QThread(this);

   // scf = new Scf("http2.scf.pumit.com", "80");

   //  scf->get_session("114731", "bertrand19002");


   // scf->get_metadata("114713");

  //  scf->get_cfo();

   // connect(t_scf, SIGNAL(started()), scf, SLOT(init()));
   // connect(scf, SIGNAL(destroyed()), t_scf, SLOT(quit()), Qt::DirectConnection);
   // scf->moveToThread(t_scf);
   // t_scf->start();

   // scf->get_session("114702", "bertrand19002");

}

Qtrackerserv::~Qtrackerserv()
{
    qDebug() << "END Qtrackerserver";
   // scf->deleteLater();
   // t_scf->wait();
}

void Qtrackerserv::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "READ incomingConnection";

    // When a new client connects, the server constructs a QTcpSocket and all
    // communication with the client is done over this QTcpSocket. QTcpSocket
    // works asynchronously, this means that all the communication is done
    // in the two slots readClient() and discardClient().

    QThread *t_tracker = new QThread(this);
    QTracker *qtracker = new QTracker(socketDescriptor, this);

    connect(t_tracker, SIGNAL(started()), qtracker, SLOT(init()));
    connect(qtracker, SIGNAL(destroyed()), t_tracker, SLOT(quit()), Qt::DirectConnection);


   // connect(scf, SIGNAL(emitSession()), streamingthread, SLOT(parse_client()), Qt::BlockingQueuedConnection);
//    connect(scf, SIGNAL(emitFrame(QByteArray)), streamingthread, SLOT(frame_recompositing(QByteArray)), Qt::BlockingQueuedConnection);
    //connect(scf, SIGNAL(emitVFrame(QByteArray)), streamingthread, SLOT(frame_Vrecompositing(QByteArray)), Qt::BlockingQueuedConnection);
    //connect(scf, SIGNAL(emitAFrame(QByteArray)), streamingthread, SLOT(frame_Arecompositing(QByteArray)), Qt::BlockingQueuedConnection);

    //connect(streamingthread, SIGNAL(emit_get_Vframe(QString,QString)), scf, SLOT(slotVFrame(QString,QString)), Qt::BlockingQueuedConnection);
    //connect(streamingthread, SIGNAL(emit_get_Aframe(QString,QString)), scf, SLOT(slotAFrame(QString,QString)), Qt::BlockingQueuedConnection);

    qtracker->moveToThread(t_tracker);
    t_tracker->start();



    //QtServiceBase::instance()->logMessage("New Connection");
}

