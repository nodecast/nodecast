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

    if (Xmpp_client::m_instance)
    {
        //delete m_instance;
        //m_instance = NULL;

        Xmpp_client::m_instance->reload(account["login"], account["password"]);
    }
    else
    {
        qDebug() << "XMPP BEFORE : " << account["login"]  << " PASS : " << account["password"];
        m_instance = new Xmpp_client(account["login"], account["password"], 5222);
        //connect(m_xmpp_client, SIGNAL(emit_tchat(QString)), SLOT(receive_tchat(QString)));
        qDebug() << "XMPP AFTER";
    }
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
    bool check;
    Q_UNUSED(check);

    m_capabilitiesCache = new capabilitiesCache(this);
    m_vCardCache = new vCardCache(this);

    qDebug() << "Xmpp_client::Xmpp_client !!!";

    QXmppLogger::getLogger()->setLoggingType(QXmppLogger::SignalLogging);


    transfer_manager = new QXmppTransferManager;
    this->addExtension (transfer_manager);


    // uncomment one of the following if you only want to use a specific transfer method:
    //
     //transfer_manager->setSupportedMethods(QXmppTransferJob::InBandMethod);
    // manager->setSupportedMethods(QXmppTransferJob::SocksMethod);
     transfer_manager->setProxy("proxy.nodecast.net");
     transfer_manager->setSupportedMethods(QXmppTransferJob::InBandMethod);

     muc_manager = new QXmppMucManager;
     this->addExtension(muc_manager);



     check = connect(&this->rosterManager(),
                          SIGNAL(rosterReceived()),
                          this, SLOT(rosterReceived()));
     Q_ASSERT(check);


     check = connect(&this->rosterManager(),
                          SIGNAL(itemChanged(QString)),
                          this, SLOT(rosterChanged(QString)));
     Q_ASSERT(check);


     check = connect(&this->rosterManager(), SIGNAL(presenceChanged(QString,QString)),
                     this, SLOT(presenceChanged(QString,QString)));                
     Q_ASSERT(check);


     check = connect(this, SIGNAL(messageReceived(const QXmppMessage&)),
           SLOT(messageReceived(const QXmppMessage&)));
       Q_ASSERT(check);
       Q_UNUSED(check);

    check = connect(this, SIGNAL(presenceReceived(const QXmppPresence&)),
                    SLOT(presenceReceived(const QXmppPresence&)));



    check = connect(this, SIGNAL(connected()), SLOT(updateStatusWidget()));
    Q_ASSERT(check);


   // check = connect(&m_xmppClient, SIGNAL(connected()), SLOT(addAccountToCache()));
  //  Q_ASSERT(check);


    check = connect(this, SIGNAL(connected()),
                    SLOT(connectedToServer()));


    check = connect(this, SIGNAL(disconnected()),
                    SLOT(disconnectedToServer()));



    check = connect(this, SIGNAL(error(QXmppClient::Error)),
                    SLOT(connectedError()));



    check = connect(transfer_manager, SIGNAL(fileReceived(QXmppTransferJob*)),
                             SLOT(file_received(QXmppTransferJob*)));



    check = connect(&this->vCardManager(),
                    SIGNAL(vCardReceived(QXmppVCardIq)), m_vCardCache,
                    SLOT(vCardReceived(QXmppVCardIq)));
    Q_ASSERT(check);

    check = connect(m_vCardCache,
                    SIGNAL(vCardReadyToUse(QString)),
                    SLOT(updateVCard(QString)));
    Q_ASSERT(check);




    check = connect(QXmppLogger::getLogger(),
            SIGNAL(message(QXmppLogger::MessageType,QString)),
            &m_consoleDlg,
            SLOT(message(QXmppLogger::MessageType,QString)));
    Q_ASSERT(check);


    /*check = connect(manager, SIGNAL(jobFinished(QXmppTransferJob*)),
                             SLOT(job_finished(QXmppTransferJob*)));
*/

    //this->logger()->setLoggingType(QXmppLogger::StdoutLogging);

    //this->logger()->setLoggingType(QXmppLogger::FileLogging);


    m_rosterItemSortFilterModel = new rosterItemSortFilterProxyModel(this);
    m_rosterItemModel = new rosterItemModel(this);
    roster = new Ui::Roster;
    roster_widget= new QWidget;
    roster->setupUi(roster_widget);
    layoutRoster = new QVBoxLayout;
    layoutRoster->addWidget(&m_statusWidget);
    layoutRoster->addWidget(roster_widget);

    user_chat = new QTextEdit;
    vRosterSplitter = new QSplitter(Qt::Horizontal);
    groupBoxContact = new QGroupBox;
    groupBoxContact->setLayout(layoutRoster);

    vRosterSplitter->addWidget(groupBoxContact);
    vRosterSplitter->addWidget(user_chat);

    check = connect(roster->lineEdit_filter, SIGNAL(textChanged(QString)),
                    this, SLOT(filterChanged(QString)));
    Q_ASSERT(check);


    m_rosterItemSortFilterModel->setSourceModel(m_rosterItemModel);
    roster->listView->setModel(m_rosterItemSortFilterModel);
    m_rosterItemSortFilterModel->sort(0);

    rosterItemDelegate *delegate = new rosterItemDelegate();
    roster->listView->setItemDelegate(delegate);
    roster->verticalLayout->insertWidget(0, &m_statusWidget);


    check = connect(&m_statusWidget, SIGNAL(statusTextChanged(QString)),
                    SLOT(statusTextChanged(QString)));
    Q_ASSERT(check);
    check = connect(&m_statusWidget, SIGNAL(presenceTypeChanged(QXmppPresence::Type)),
                    SLOT(presenceTypeChanged(QXmppPresence::Type)));
    Q_ASSERT(check);
    check = connect(&m_statusWidget,
                    SIGNAL(presenceStatusTypeChanged(QXmppPresence::AvailableStatusType)),
                    SLOT(presenceStatusTypeChanged(QXmppPresence::AvailableStatusType)));
    Q_ASSERT(check);
    check = connect(&m_statusWidget,
                    SIGNAL(avatarChanged(QImage)),
                    SLOT(avatarChanged(QImage)));
    Q_ASSERT(check);




    this->configuration().setPort(m_xmpp_client_port);
    this->configuration().setJid(m_login);
    this->configuration().setPassword(m_password);
    this->configuration().setResource("nodecast");
    this->configuration().setStreamSecurityMode(QXmppConfiguration::StreamSecurityMode::TLSRequired);

    this->connectToServer(this->configuration());
}


Xmpp_client::~Xmpp_client()
{
    qDebug() << "Xmpp_client shutdown";
    m_consoleDlg.deleteLater();
    if (!connection_failed) this->disconnectFromServer ();
    qDebug() << "Xmpp_client : close socket";
    //log->close();

   // z_push_api->close ();
   // delete(z_push_api);
}


void Xmpp_client::show_xml_console()
{
    m_consoleDlg.show();
}


void Xmpp_client::reload(QString login, QString password)
{


    foreach(QXmppMucRoom* room, rooms)
    {
        if (room->isJoined()) room->leave("I'll be back");
        qDebug() << "DELETE ROOM :" << room->name();
        delete room;
    }
    rooms.clear();


    m_instance->disconnectFromServer();

    m_login = login;
    m_password = password;
    this->configuration().setJid(m_login);
    this->configuration().setPassword(m_password);
    this->configuration().setResource("nodecast");
    this->configuration().setStreamSecurityMode(QXmppConfiguration::StreamSecurityMode::TLSRequired);

    this->connectToServer(this->configuration());
}



void Xmpp_client::init()
{
    log = new QFile("/tmp/xmppclient");
     if (!log->open(QIODevice::Append | QIODevice::Text))
             return;

}

/*
void Xmpp_client::addRoster(const QString jid)
{
    qDebug() << "add to roster : " << jid;
    roster_manager->addItem(jid);
}
*/

void Xmpp_client::sendMessage(const QString jid, const QString message)
{
    qDebug() << "send message to : " << jid << " MESSAGE : " << message;
    this->sendMessage(jid, message);
    return;
}


void Xmpp_client::sendFile(const QString jid, const QString path)
{
    qDebug() << "send file : " << path << " TO : " << jid;
    transfer_manager->sendFile(jid, path, "FILE DESCRIPTION TEST");
}


void Xmpp_client::file_received (QXmppTransferJob *job)
{
    file_name = job->fileName();
    QString from_jid = job->jid();
    qint64 file_size = job->fileSize();

    qDebug() << "Xmpp_client::file_received : " << file_name << " file size : " << file_size;
    qDebug() << "Got transfer request from:" << job->jid();
    //Got transfer request from: "potes_93c8d9761e1f4c85829f46c354a723c4@conference.nodecast.net/test"


    if (!from_jid.contains("_") || !from_jid.contains("@") || !from_jid.contains("/"))
    {
        qDebug() << "FILE RECEIVED : MALFORMED JID";
        job->abort();
        return;
    }

    QXmppTransferFileInfo fileInfo = job->fileInfo();

    qDebug() << "FILE INFO DESC : " << fileInfo.description();

    // check extension
    if (!file_name.contains("."))
    {
        qDebug() << "file without .";
        job->abort();
        return;
    }
    else if (file_size > 3000000 || file_size == 0)
    {
        qDebug() << "file too big > 3 Mo";
        job->abort();
        return;
    }
    QString extension = file_name.split(".").takeLast();
    qDebug() << "RECEIVE FILE WITH EXTENSION : " << extension;

    if (extension != "torrent")
    {
        qDebug() << "file without .torrent";
        job->abort();
        return;
    }


    sphere_dest = from_jid.split("@").at(0);
    QString from = from_jid.split("/").at(1);

    qDebug() << "RECEIVE from : " << from << " TO : " << sphere_dest;


    // mkdir from user directory to torrents directory to not resend file (bypass filesystemwatcher)
    file_dir = new QDir(prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_dest + "/torrents/" + from);
    if (!file_dir->exists()) file_dir->mkpath(prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_dest + "/torrents/" + from);



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
/*
    QDir sphere_dir;
    sphere_dir = prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_dest + "/";
    if (!sphere_dir.exists())
    {
        qDebug() << "SPHERE DEST NOT EXIST : " << QDir::toNativeSeparators(sphere_dir.absolutePath());
        m_buffer->close ();
        delete m_buffer;
        delete file;
        return;
    }
*/
    qDebug() << "WRITE : " << file_name << " TO " <<  QDir::toNativeSeparators(file_dir->absolutePath());

    QFile file_torrent(QDir::toNativeSeparators(file_dir->absolutePath() + QDir::separator()) + file_name);
    if (!file_torrent.open(QIODevice::WriteOnly))
        return;

    file_torrent.write(m_buffer->data());

    QFileInfo file_info(file_torrent.fileName());
    QString file_path = file_info.canonicalFilePath();
    qDebug() << "FILE PATH : " << file_path;
    file_torrent.close();
    m_buffer->close ();
    delete m_buffer;
    delete file_dir;

    // archive file to torrents directory
    //QDir nodecast_datas;
    //nodecast_datas = prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_dest + "/torrents/";
    //QFile::copy(file_info.absoluteFilePath(), nodecast_datas.absolutePath() + QDir::separator() + file_info.fileName() );

    if (fsutils::isValidTorrentFile(file_path))
        QBtSession::instance()->addTorrent(file_path);
    else
    {
        qDebug() << "RECEIVE A NOT VALID TORRENT : " << file_path;
        fsutils::forceRemove(file_path);
    }
}





void Xmpp_client::connectedError()
{
    qDebug() << "Xmpp_client::connectedError";
    qDebug() << "Connection failed !";
    connection_failed = true;
}

void Xmpp_client::connectedToServer()
{
    connection_failed = false;
    qDebug() << "Xmpp_client::connectedToServer";
    qDebug() << "Connection successfull !";

    rooms.insert("nodecast", muc_manager->addRoom("nodecast@conference.nodecast.net"));
    rooms["nodecast"]->setNickName(Preferences().getNodecastNickname());
    rooms["nodecast"]->join();

    emit emit_connected(true);
}


void Xmpp_client::disconnectedToServer()
{
    connection_failed = true;
    qDebug() << "Xmpp_client::disconnectedToServer";


    foreach(QXmppMucRoom* room, rooms)
    {
        qDebug() << "DELETE ROOM :" << room->name();
        delete room;
    }
    rooms.clear();
    emit emit_connected(false);
}


void Xmpp_client::connectToRoom(QString room_name)
{
    qDebug() << "Xmpp_client::connectToRoom : " << room_name;

    if (room_name.isEmpty()) return;

    rooms.insert(room_name, muc_manager->addRoom(room_name + "@conference.nodecast.net"));
    rooms[room_name]->setNickName(Preferences().getNodecastNickname());
    rooms[room_name]->join();
    emit emit_room(room_name, rooms[room_name]);
}


void Xmpp_client::messageReceived(const QXmppMessage& message)
{
    qDebug() << "Xmpp_client::messageReceived !!!";

    QString from = message.from();
    QString invitation = message.mucInvitationJid();
    QString reason = message.mucInvitationReason();

    qDebug() << "INVITATION : " << invitation;
    qDebug() << "REASON : " << reason;


    qDebug() << "RECEIVE from : " << from << " MESSAGE : " << message.body();

    if (!invitation.isEmpty())
        emit_invitation(invitation, from, reason);
    else if (message.body().size() !=0)
        emit_chat(message.from(), message.body());
}






void Xmpp_client::presenceReceived(const QXmppPresence& presence)
{
    QString from = presence.from();
    qDebug() << "Xmpp_client::presenceReceived !!! : " << from;

    QString message;
    switch(presence.type())
    {
    case QXmppPresence::Subscribe:
        {
            message = "<B>%1</B> wants to subscribe";

            int retButton = QMessageBox::question(
                    new QWidget, "Contact Subscription", message.arg(from),
                    QMessageBox::Yes, QMessageBox::No);


            switch(retButton)
            {
            case QMessageBox::Yes:
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
                }
                break;
            case QMessageBox::No:
                {
                    QXmppPresence unsubscribed;
                    unsubscribed.setTo(from);
                    unsubscribed.setType(QXmppPresence::Unsubscribed);
                    this->sendPacket(unsubscribed);
                }
                break;
            default:
                break;
            }
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

    QMessageBox::information(new QWidget, "Contact Subscription", message.arg(from),
            QMessageBox::Ok);
}





void Xmpp_client::presenceChanged(const QString& bareJid, const QString& resource)
{
    if(bareJid == this->configuration().jidBare())
        return;

    if(!m_rosterItemModel->getRosterItemFromBareJid(bareJid))
        return;

    QString jid = bareJid + "/" + resource;
    QMap<QString, QXmppPresence> presences = this->rosterManager().getAllPresencesForBareJid(bareJid);
    m_rosterItemModel->updatePresence(bareJid, presences);

    QXmppPresence& pre = presences[resource];

    if(pre.type() == QXmppPresence::Available)
    {
        QString node = pre.capabilityNode();
        QString ver = pre.capabilityVer().toBase64();
        QStringList exts = pre.capabilityExt();

        QString nodeVer = node + "#" + ver;
        if(!m_capabilitiesCache->isCapabilityAvailable(nodeVer))
            m_capabilitiesCache->requestInfo(jid, nodeVer);

        foreach(QString ext, exts)
        {
            nodeVer = node + "#" + ext;
            if(!m_capabilitiesCache->isCapabilityAvailable(nodeVer))
                m_capabilitiesCache->requestInfo(jid, nodeVer);
        }

        switch(pre.vCardUpdateType())
        {
        case QXmppPresence::VCardUpdateNone:
            if(!m_vCardCache->isVCardAvailable(bareJid))
                m_vCardCache->requestVCard(bareJid);
        case QXmppPresence::VCardUpdateNotReady:
            break;
        case QXmppPresence::VCardUpdateNoPhoto:
        case QXmppPresence::VCardUpdateValidPhoto:
            if(m_vCardCache->getPhotoHash(bareJid) != pre.photoHash())
                m_vCardCache->requestVCard(bareJid);
            break;
        }
    }

//    QXmppPresence::Type presenceType = presences.begin().value().getType();

//    if(!m_vCardCache.isVCardAvailable(bareJid) &&
//       presenceType == QXmppPresence::Available)
//    {
//        m_rosterItemModel.updateAvatar(bareJid,
//                                   m_vCardCache.getVCard(bareJid).image);
//    }
}



void Xmpp_client::action_removeContact(const QString& bareJid)
{
    if(!isValidBareJid(bareJid))
        return;

    int answer = QMessageBox::question(new QWidget, "Remove contact",
                            QString("Do you want to remove the contact <I>%1</I>").arg(bareJid),
                            QMessageBox::Yes, QMessageBox::No);
    if(answer == QMessageBox::Yes)
    {
        QXmppRosterIq remove;
        remove.setType(QXmppIq::Set);
        QXmppRosterIq::Item itemRemove;
        itemRemove.setSubscriptionType(QXmppRosterIq::Item::Remove);
        itemRemove.setBareJid(bareJid);
        remove.addItem(itemRemove);
        this->sendPacket(remove);

        m_rosterItemModel->removeRosterEntry(bareJid);
    }
}




void Xmpp_client::addPhotoHash(QXmppPresence& pre)
{
    QString clientBareJid = configuration().jidBare();

    if(m_vCardCache->isVCardAvailable(clientBareJid))
    {
        QByteArray hash = m_vCardCache->getPhotoHash(clientBareJid);
        if(hash.isEmpty())
            pre.setVCardUpdateType(QXmppPresence::VCardUpdateNoPhoto);
        else
            pre.setVCardUpdateType(QXmppPresence::VCardUpdateValidPhoto);
        pre.setPhotoHash(hash);
    }
    else
    {
        pre.setVCardUpdateType(QXmppPresence::VCardUpdateNone);
        pre.setPhotoHash(QByteArray());
    }
}




void Xmpp_client::statusTextChanged(const QString& status)
{
    QXmppPresence presence = clientPresence();
    presence.setStatusText(status);
    addPhotoHash(presence);
    setClientPresence(presence);
}



void Xmpp_client::filterChanged(const QString& filter)
{
    m_rosterItemSortFilterModel->setFilterRegExp(filter);

    // follow statement selects the first row
    roster->listView->selectionModel()->select(roster->listView->model()->index(0, 0),
                                           QItemSelectionModel::ClearAndSelect);
}



void Xmpp_client::rosterReceived()
{
    QStringList list = this->rosterManager().getRosterBareJids();
    QString bareJid;
    foreach(bareJid, list)
        rosterChanged(bareJid);
}


void Xmpp_client::rosterChanged(const QString& bareJid)
{
    m_rosterItemModel->updateRosterEntry(bareJid, this->rosterManager().
                                        getRosterEntry(bareJid));

    // if available in cache, update it else based on presence it will request if not available
    if(m_vCardCache->isVCardAvailable(bareJid))
        updateVCard(bareJid);
}



void Xmpp_client::updateVCard(const QString& bareJid)
{
    qDebug() << "UPDATE VCARD : " << bareJid;
    // determine full name
    const QXmppVCardIq vCard = m_vCardCache->getVCard(bareJid);
    QString fullName = vCard.fullName();
    if (fullName.isEmpty())
        fullName = bareJid;

    // determine avatar
    QImage avatar = m_vCardCache->getAvatar(bareJid);
    if (avatar.isNull())
        avatar = QImage(":/xmpp/resources/avatar.png");

    if (bareJid == this->configuration().jidBare()) {
        // update our own information
        m_statusWidget.setAvatar(avatar);
        m_statusWidget.setDisplayName(fullName);
    } else {
        // update roster information
        m_rosterItemModel->updateAvatar(bareJid, avatar);
        m_rosterItemModel->updateName(bareJid, fullName);
    }
}




void Xmpp_client::presenceTypeChanged(QXmppPresence::Type presenceType)
{
    if(presenceType == QXmppPresence::Unavailable)
    {
        this->disconnectFromServer();
    }
    else if(presenceType == QXmppPresence::Available)
    {
        QXmppPresence newPresence = this->clientPresence();
        newPresence.setType(presenceType);
        newPresence.setAvailableStatusType(QXmppPresence::Online);
        addPhotoHash(newPresence);
        this->setClientPresence(newPresence);
    }
    m_statusWidget.setStatusText(
            presenceToStatusText(this->clientPresence()));
}


void Xmpp_client::presenceStatusTypeChanged(QXmppPresence::AvailableStatusType statusType)
{
    QXmppPresence presence = this->clientPresence();
    presence.setType(QXmppPresence::Available);
    presence.setAvailableStatusType(statusType);
    addPhotoHash(presence);
    this->setClientPresence(presence);
    m_statusWidget.setStatusText(
            presenceToStatusText(this->clientPresence()));
}



void Xmpp_client::avatarChanged(const QImage& image)
{
    QXmppVCardIq vcard;
    vcard.setType(QXmppIq::Set);

    QByteArray ba;
    QBuffer buffer(&ba);
    if(buffer.open(QIODevice::WriteOnly))
    {
        if(image.save(&buffer, "PNG"))
        {
            vcard.setPhoto(ba);
            this->sendPacket(vcard);
            m_statusWidget.setAvatar(image);

            m_vCardCache->getVCard(this->configuration().jidBare()) = vcard;
            // update photo hash
            QXmppPresence presence = this->clientPresence();
            addPhotoHash(presence);
            this->setClientPresence(presence);
        }
    }
}




void Xmpp_client::updateStatusWidget()
{
    const QString bareJid = this->configuration().jidBare();

    // initialise status widget
    updateVCard(bareJid);
    m_statusWidget.setStatusText(presenceToStatusText(this->clientPresence()));
    m_statusWidget.setPresenceAndStatusType(this->clientPresence().type(),
                                            this->clientPresence().availableStatusType());

    // fetch own vCard
    m_vCardCache->requestVCard(bareJid);
}
