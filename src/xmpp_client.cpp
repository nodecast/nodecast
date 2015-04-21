#include "xmpp_client.h"

Xmpp_client* Xmpp_client::m_instance = NULL;
bool Xmpp_client::connection_failed = false;


Xmpp_client * Xmpp_client::instance()
{
  if (Xmpp_client::m_instance)
      return m_instance;
}



void Xmpp_client::connectXMPP()
{
    QHash<QString, QString> account;
    Preferences prefs;
    account = prefs.getNodecastAccount();

    if (Xmpp_client::m_instance && !account.isEmpty())
    {
        //delete m_instance;
        //m_instance = NULL;

        Xmpp_client::m_instance->reload(account["login"], account["password"]);
    }
    else if (!Xmpp_client::m_instance && !account.isEmpty())
    {
        qDebug() << "XMPP BEFORE";
        m_instance = new Xmpp_client(account["login"], account["password"], 5223);
        //connect(m_xmpp_client, SIGNAL(emit_tchat(QString)), SLOT(receive_tchat(QString)));
        qDebug() << "XMPP AFTER";
    }
    else qDebug() << "CONFIG IS EMPTY";
}

void Xmpp_client::drop()
{
  if (Xmpp_client::m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}


Xmpp_client::Xmpp_client(QString a_login, QString a_password, int a_xmpp_client_port, QObject *parent) : m_login(a_login), m_password(a_password), m_xmpp_client_port(a_xmpp_client_port), QXmppClient(parent)
{
    qDebug() << "Xmpp_client::Xmpp_client !!!";

    transfer_manager = new QXmppTransferManager;
    this->addExtension (transfer_manager);


    // uncomment one of the following if you only want to use a specific transfer method:
    //
     transfer_manager->setSupportedMethods(QXmppTransferJob::InBandMethod);
    // manager->setSupportedMethods(QXmppTransferJob::SocksMethod);
     transfer_manager->setProxy("proxy.nodecast.net");

     muc_manager = new QXmppMucManager;
     this->addExtension(muc_manager);



    bool check = connect(this, SIGNAL(messageReceived(const QXmppMessage&)),
           SLOT(messageReceived(const QXmppMessage&)));
       Q_ASSERT(check);
       Q_UNUSED(check);

    check = connect(this, SIGNAL(presenceReceived(const QXmppPresence&)),
                    SLOT(presenceReceived(const QXmppPresence&)));


    check = connect(this, SIGNAL(connected()),
                    SLOT(connectedToServer()));


    check = connect(this, SIGNAL(error(QXmppClient::Error)),
                    SLOT(connectedError()));



    check = connect(transfer_manager, SIGNAL(fileReceived(QXmppTransferJob*)),
                             SLOT(file_received(QXmppTransferJob*)));


    /*check = connect(manager, SIGNAL(jobFinished(QXmppTransferJob*)),
                             SLOT(job_finished(QXmppTransferJob*)));
*/

    //this->logger()->setLoggingType(QXmppLogger::StdoutLogging);

    //this->logger()->setLoggingType(QXmppLogger::FileLogging);

    this->configuration().setPort(m_xmpp_client_port);
    this->configuration().setJid(m_login);
    this->configuration().setPassword(m_password);
    this->configuration().setResource("nodecast");

    this->connectToServer(this->configuration());

    //this->connectToServer("ncs@localhost/server", "scn");


   /* subscribe.setTo("geekast@localhost");
    subscribe.setType(QXmppPresence::Subscribe);
    this->sendPacket(subscribe);*/
}


Xmpp_client::~Xmpp_client()
{
    qDebug() << "Xmpp_client shutdown";
    if (!connection_failed) this->disconnectFromServer ();
    qDebug() << "Xmpp_client : close socket";
    //log->close();

   // z_push_api->close ();
   // delete(z_push_api);
}


void Xmpp_client::reload(QString login, QString password)
{
    m_instance->disconnectFromServer();

    m_login = login;
    m_password = password;
    this->configuration().setJid(m_login);
    this->configuration().setPassword(m_password);
    this->configuration().setResource("nodecast");
    this->connectToServer(this->configuration());
}



void Xmpp_client::init()
{
    log = new QFile("/tmp/xmppclient");
     if (!log->open(QIODevice::Append | QIODevice::Text))
             return;

}

void Xmpp_client::sendFile(QString jid, QString path)
{
    qDebug() << "send file : " << path << " TO : " << jid;
    transfer_manager->sendFile(jid, path);


}


void Xmpp_client::file_received (QXmppTransferJob *job)
{
    qDebug() << "Xmpp_client::file_received : " << job->fileName() << " file size : " << job->fileSize();
    qDebug() << "Got transfer request from:" << job->jid();


    file_name = job->fileName();

    bool check = connect(job, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(job_error(QXmppTransferJob::Error)));
    Q_ASSERT(check);

    check = connect(job, SIGNAL(finished()), this, SLOT(job_finished()));
    Q_ASSERT(check);

    check = connect(job, SIGNAL(progress(qint64,qint64)), this, SLOT(job_progress(qint64,qint64)));
    Q_ASSERT(check);

    // allocate a buffer to receive the file
    m_buffer = new QBuffer(this);
    m_buffer->open(QIODevice::WriteOnly);
    job->accept(m_buffer);
}

void Xmpp_client::job_error(QXmppTransferJob::Error error)
{
    qDebug() << "Transmission failed : " << error;
}

/// A file transfer has made progress.

void Xmpp_client::job_progress(qint64 done, qint64 total)
{
    qDebug() << "Transmission progress:" << done << "/" << total;
}


void Xmpp_client::job_finished ()
{
    qDebug() << "Xmpp_client::job_finished";

    //m_buffer->write("/tmp/xmpp_file");

    file = new QFile("/tmp/" + file_name);
    file->open(QIODevice::WriteOnly);
    file->write(m_buffer->data());
    file->close();
    m_buffer->close ();
    m_buffer->deleteLater();
    file->deleteLater();
}





void Xmpp_client::connectedError()
{
    qDebug() << "Xmpp_client::connectedError";
    qDebug() << "Connection failed !";
    connection_failed = true;
    emit emit_connected(false);
}

void Xmpp_client::connectedToServer()
{
    connection_failed = false;
    qDebug() << "Xmpp_client::connectedToServer";
    qDebug() << "Connection successfull !";

    rooms.insert("nodecast", muc_manager->addRoom("nodecast@conference.nodecast.net"));
    rooms["nodecast"]->setNickName(Preferences().getNodecastLogin());
    rooms["nodecast"]->join();

    emit emit_connected(true);
}



void Xmpp_client::connectToRoom(QString room_name)
{
    qDebug() << "Xmpp_client::connectToRoom : " << room_name;

    rooms.insert(room_name, muc_manager->addRoom(room_name + "@conference.nodecast.net"));
    rooms[room_name]->setNickName(Preferences().getNodecastLogin());
    rooms[room_name]->join();
    emit emit_room(room_name, rooms[room_name]);
}


void Xmpp_client::messageReceived(const QXmppMessage& message)
{
    qDebug() << "Xmpp_client::messageReceived !!!";

    QString from = message.from();

    //QByteArray msg = QByteArray::fromBase64(message.body().toUtf8());


    qDebug() << "RECEIVE from : " << from << " MESSAGE : " << message.body();


    if (message.body().size() !=0) emit_chat(message.from(), message.body());
}






void Xmpp_client::presenceReceived(const QXmppPresence& presence)
{
    qDebug() << "Xmpp_client::presenceReceived !!!";
    QString from = presence.from();

    QString message;
    switch(presence.type())
    {
    case QXmppPresence::Subscribe:
        {
            QXmppPresence subscribed;
            subscribed.setTo(from);

            subscribed.setType(QXmppPresence::Subscribed);
            this->sendPacket(subscribed);

            // reciprocal subscription
            QXmppPresence subscribe;
            subscribe.setTo(from);
            subscribe.setType(QXmppPresence::Subscribe);
            this->sendPacket(subscribe);

            return;
        }
        break;
    case QXmppPresence::Subscribed:
        message = "<B>%1</B> accepted your request";
        break;
    case QXmppPresence::Unsubscribe:
        message = "<B>%1</B> unsubscribe";
        break;
    case QXmppPresence::Unsubscribed:
        message = "<B>%1</B> unsubscribed";
        break;
    default:
        return;
        break;
    }

    if(message.isEmpty())
        return;
}
