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
    Preferences* const pref = Preferences::instance();
    account = pref->getNodecastAccount();

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
    Q_ASSERT(check);


    check = connect(this, SIGNAL(disconnected()),
                    SLOT(disconnectedToServer()));
    Q_ASSERT(check);


    check = connect(this, SIGNAL(error(QXmppClient::Error)),
                    SLOT(connectedError()));
    Q_ASSERT(check);



    check = connect(transfer_manager, SIGNAL(fileReceived(QXmppTransferJob*)),
                             SLOT(file_received(QXmppTransferJob*)));
    Q_ASSERT(check);


    check = connect(transfer_manager, SIGNAL(jobFinished(QXmppTransferJob*)),
                             SLOT(job_finished(QXmppTransferJob*)));
    Q_ASSERT(check);


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

    m_stacked_tab_chat = new QStackedWidget;

    vRosterSplitter = new QSplitter(Qt::Horizontal);
    groupBoxContact = new QGroupBox;
    groupBoxContact->setLayout(layoutRoster);

    vRosterSplitter->addWidget(groupBoxContact);
    vRosterSplitter->addWidget(m_stacked_tab_chat);

    check = connect(roster->lineEdit_filter, SIGNAL(textChanged(QString)),
                    this, SLOT(filterChanged(QString)));
    Q_ASSERT(check);


    m_rosterItemSortFilterModel->setSourceModel(m_rosterItemModel);
    roster->listView->setModel(m_rosterItemSortFilterModel);
    m_rosterItemSortFilterModel->sort(0);


    connect(roster->listView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(rosterItemSelectionChanged(QItemSelection)));




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


    check = connect(roster->listView, SIGNAL(showProfile(QString)),
                    this, SLOT(showProfile(QString)));
    Q_ASSERT(check);

    check = connect(roster->listView, SIGNAL(removeContact(QString)),
                    this, SLOT(action_removeContact(QString)));
    Q_ASSERT(check);

    check = connect(roster->pushButton_addContact, SIGNAL(clicked()), SLOT(action_addContact()));
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


void Xmpp_client::sendFile(const QString jid, const QString path)
{
    qDebug() << "send file : " << path << " TO : " << jid;
    transfer_manager->sendFile(jid, path, "FILE DESCRIPTION TEST");
}


void Xmpp_client::file_received (QXmppTransferJob *job)
{
    QString file_name = job->fileName();
    QString from_jid = job->jid();
    qint64 file_size = job->fileSize();
    Preferences* const pref = Preferences::instance();

    qDebug() << "Xmpp_client::file_received : " << file_name << " file size : " << file_size;
    qDebug() << "Got transfer request from:" << job->jid();
    //Got transfer request from: "potes_93c8d9761e1f4c85829f46c354a723c4@conference.nodecast.net/test"


    if (!from_jid.contains("_") || !from_jid.contains("@") || !from_jid.contains("/"))
    {
        qDebug() << "FILE RECEIVED : MALFORMED JID";
        job->abort();
        return;
    }
    QString sphere_dest = from_jid.split("@").at(0);
    QString from = from_jid.split("/").at(1);

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
    //QString extension = file_name.split(".").takeLast();
    QString file_extension = fsutils::fileExtension(file_name);
    qDebug() << "RECEIVE FILE WITH EXTENSION : " << file_extension;


    if (file_extension.isEmpty())
    {
        qDebug() << "file without extension";
        job->abort();
        return;
    }
    else if (file_extension != "torrent" && file_size > 1000000)
    {
        qDebug() << "file size > 1Mo and not torrent";
        job->abort();
        return;
    }
    else if (file_extension != "torrent" && file_size < 1000000)
    {

        file_dir = new QDir(pref->getSavePath() + "/nodecast/spheres/private/" + sphere_dest);
        if (!file_dir->exists()) file_dir->mkpath(pref->getSavePath() + "/nodecast/spheres/private/" + sphere_dest);

         global_mutex::thumbnail_mutex.lock();

        emit emit_receive_rawfile(sphere_dest, file_dir->absoluteFilePath(file_name), job);
    }
    else
    {
        // mkdir from user directory to torrents directory to not resend file (bypass filesystemwatcher)
        file_dir = new QDir(pref->getSavePath() + "/nodecast/spheres/private/" + sphere_dest + "/torrents/" + from);
        if (!file_dir->exists()) file_dir->mkpath(pref->getSavePath() + "/nodecast/spheres/private/" + sphere_dest + "/torrents/" + from);
    }



    qDebug() << "RECEIVE from : " << from << " TO : " << sphere_dest;




    bool check = connect(job, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(job_error(QXmppTransferJob::Error)));
    Q_ASSERT(check);



    File_data file_data;
    file_data.file_extension = file_extension;
    file_data.buffer = new QBuffer(this);
    file_data.file_name = file_name;
    file_data.file_dir = file_dir;
    file_buffer.insert(job->sid(), file_data);

    // allocate a buffer to receive the file
    file_data.buffer->open(QIODevice::WriteOnly);
    job->accept(file_data.buffer);
}

void Xmpp_client::job_error(QXmppTransferJob::Error error)
{
    qDebug() << "Transmission failed : " << error;
    global_mutex::thumbnail_mutex.unlock();
}



void Xmpp_client::job_finished (QXmppTransferJob *job)
{

    if (!file_buffer.contains(job->sid()))
    {
        qDebug() << "Xmpp_client::job_finished NOT FOUND : " << job->sid();
        job->deleteLater();
        return;
    }


    qDebug() << "Xmpp_client::job_finished : " << file_buffer.value(job->sid()).file_name;


    if (file_buffer.value(job->sid()).file_extension == "torrent")
    {
        qDebug() << "Xmpp_client::job_finished WRITE : " << file_buffer.value(job->sid()).file_name << " TO " <<  QDir::toNativeSeparators(file_dir->absolutePath()+ QDir::separator() + file_buffer.value(job->sid()).file_name);

        QFile file_torrent(file_buffer.value(job->sid()).file_dir->absolutePath() + QDir::separator() + file_buffer.value(job->sid()).file_name);
        if (!file_torrent.open(QIODevice::WriteOnly))
        {
            file_buffer.value(job->sid()).buffer->close();
            delete file_buffer.value(job->sid()).buffer;
            delete file_buffer.value(job->sid()).file_dir;
            file_buffer.remove(job->sid());

            return;
        }

        file_torrent.write(file_buffer.value(job->sid()).buffer->data());
        file_torrent.close();
        file_buffer.value(job->sid()).buffer->close();

        QFileInfo file_info(file_torrent.fileName());
        QString file_path = file_info.canonicalFilePath();
        qDebug() << "FILE PATH : " << file_path;
        delete file_buffer.value(job->sid()).buffer;
        delete file_buffer.value(job->sid()).file_dir;
        file_buffer.remove(job->sid());

        // archive file to torrents directory
        //QDir nodecast_datas;
        //nodecast_datas = pref->getSavePath() + "/nodecast/spheres/private/" + sphere_dest + "/torrents/";
        //QFile::copy(file_info.absoluteFilePath(), nodecast_datas.absolutePath() + QDir::separator() + file_info.fileName() );


        if (fsutils::isValidTorrentFile(file_path))
            QBtSession::instance()->addTorrent(file_path);
    }
    else if (file_buffer.value(job->sid()).file_extension != "torrent")
    {
        qDebug() << "Xmpp_client::job_finished WRITE RAW : " <<  file_buffer.value(job->sid()).file_name << " TO " <<  QDir::toNativeSeparators(file_buffer.value(job->sid()).file_dir->absolutePath()+ QDir::separator() + file_buffer.value(job->sid()).file_name);
        QString file_path = file_buffer.value(job->sid()).file_dir->absolutePath() + QDir::separator() + file_buffer.value(job->sid()).file_name;
        QFile file_raw(file_path);

        if (!file_raw.open(QIODevice::WriteOnly))
        {
            file_buffer.value(job->sid()).buffer->close();
            delete file_buffer.value(job->sid()).buffer;
            delete file_buffer.value(job->sid()).file_dir;
            file_buffer.remove(job->sid());

            return;
        }
        file_raw.write(file_buffer.value(job->sid()).buffer->data());
        file_raw.close();
        file_buffer.value(job->sid()).buffer->close();
        delete file_buffer.value(job->sid()).buffer;
        delete file_buffer.value(job->sid()).file_dir;
        file_buffer.remove(job->sid());

        global_mutex::thumbnail_mutex.unlock();
    }
    else
    {
        qDebug() << "RECEIVE A NOT VALID TORRENT : " << file_buffer.value(job->sid()).file_name;
        file_buffer.value(job->sid()).buffer->close();
        delete file_buffer.value(job->sid()).buffer;
        delete file_buffer.value(job->sid()).file_dir;
        file_buffer.remove(job->sid());
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

/*    rooms.insert("nodecast", muc_manager->addRoom("nodecast@conference.nodecast.net"));
    rooms["nodecast"]->setNickName(Preferences().getNodecastNickname());
    rooms["nodecast"]->join();
*/
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
    rooms[room_name]->setNickName(Preferences::instance()->getNodecastNickname());
    rooms[room_name]->join();
    emit emit_room(room_name, rooms[room_name]);
}


bool Xmpp_client::deleteRoom(QString room_name)
{
    qDebug() << "Xmpp_client::deleteRoom : " << room_name;

    if (room_name.isEmpty() || !rooms.contains(room_name)) return false;

    if (rooms.value(room_name)->isJoined())
        rooms.value(room_name)->leave("I'll be back");
    rooms[room_name]->deleteLater();
    rooms.remove(room_name);
    return true;
}



void Xmpp_client::messageReceived(const QXmppMessage& message)
{
    qDebug() << "Xmpp_client::messageReceived !!!";

    QString from = message.from();
    QString jid = QXmppUtils::jidToBareJid(message.from());
    QString invitation = message.mucInvitationJid();
    QString reason = message.mucInvitationReason();

    qDebug() << "INVITATION : " << invitation;
    qDebug() << "REASON : " << reason;


    qDebug() << "RECEIVE from : " << from << " MESSAGE : " << message.body();

    if (!invitation.isEmpty())
        emit_invitation(invitation, from, reason);
    else if (message.body().size() !=0)
    {
        // if from is not in the roster it should be a message room
        if (this->rosterManager().getRosterEntry(jid).bareJid().isEmpty())
            emit_chat(message.from(), message.body());
        else if (itemMapChat.contains(jid))
        {
            int stacked_index = itemMapChat.value(jid);
            //chatroomMap.value(stacked_index)->append(QXmppUtils::jidToUser(jid) + " : " + message.body());
            chatroomMap.value(stacked_index)->addMessage(QXmppUtils::jidToUser(jid), message.body());
        }
    }
}






void Xmpp_client::presenceReceived(const QXmppPresence& presence)
{
    QString from = presence.from();
    //qDebug() << "Xmpp_client::presenceReceived !!! : " << from;

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

    //qDebug() << "ITEM JID : " << bareJid;

//    m_rosterItemModel->indexFromItem()
    new_chat(bareJid);
}



void Xmpp_client::updateVCard(const QString& bareJid)
{
    //qDebug() << "UPDATE VCARD : " << bareJid;
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



void Xmpp_client::showProfile(const QString& bareJid)
{
    if(bareJid.isEmpty())
        return;

    profileDialog dlg(new QWidget, bareJid, this, m_capabilitiesCache);
    dlg.setBareJid(bareJid);
    // TODO use original image
    if(!m_vCardCache->getAvatar(bareJid).isNull())
        dlg.setAvatar(m_vCardCache->getAvatar(bareJid));
    QStringList resources = this->rosterManager().getResources(bareJid);

    dlg.setFullName(m_vCardCache->getVCard(bareJid).fullName());

    if(m_vCardCache->getVCard(bareJid).fullName().isEmpty())
        dlg.setFullName(this->rosterManager().getRosterEntry(bareJid).name());

    dlg.exec();
}


void Xmpp_client::new_chat(const QString& bareJid)
{
    //qDebug() << "NEW USER CHAT WITH : " << bareJid;
    QGroupBox *groupBox = new QGroupBox;
    QVBoxLayout *box_chat = new QVBoxLayout;

    QLabel *jid = new QLabel;
    jid->setText(bareJid);


    chatGraphicsView* m_view = new chatGraphicsView;
    chatGraphicsScene* m_scene = new chatGraphicsScene;
    m_view->setChatGraphicsScene(m_scene);

    QFont font;
    font.setBold(true);
    QFontMetrics fontMetrics(font);
    QRect rect = fontMetrics.boundingRect(bareJid);
    int width = rect.width();
    m_scene->setBoxStartLength(width);



    QTextEdit *chat_room = new QTextEdit;
    chat_room->setAcceptRichText(true);
    chat_room->setReadOnly(true);
    QLineEdit *line_chat = new QLineEdit;
    connect(line_chat, SIGNAL(returnPressed()), this, SLOT(sendMessageToJid()));

    box_chat->addWidget(jid);
    //box_chat->addWidget(chat_room);
    box_chat->addWidget(m_view);

    box_chat->addWidget(line_chat);
    groupBox->setLayout(box_chat);

    int tab_index = m_stacked_tab_chat->addWidget(groupBox);
    itemMapChat.insert(bareJid, tab_index);
    //chatroomMap.insert(tab_index, chat_room);
    chatroomMap.insert(tab_index, m_view);
    linechatMap.insert(tab_index, line_chat);
}


void Xmpp_client::rosterItemSelectionChanged(const QItemSelection& selection)
{
   if(!selection.indexes().isEmpty())
   {
       QString jid = selection.indexes().first().data(rosterItem::BareJid).toString();
       if (itemMapChat.contains(jid)) m_stacked_tab_chat->setCurrentIndex(itemMapChat.value(jid));
   }
}



void Xmpp_client::sendMessageToJid()
{
    QString jid = itemMapChat.key(m_stacked_tab_chat->currentIndex());
    int stacked_index = m_stacked_tab_chat->currentIndex();

    QString message = linechatMap.value(stacked_index)->text();
    if (message.isEmpty()) return;

    if (message.startsWith("/msg"))
    {
        QStringList msg_dest = message.split(" ");
        qDebug() << "DEST : " << msg_dest << " SIZE : " << msg_dest.size();
        if (msg_dest.size() != 3)
        {
            linechatMap.value(stacked_index)->clear();
        }
        else
        {
            /*
            msg_dest.removeFirst();

            QString dest = msg_dest.takeFirst() + "@";

            QList<QListWidgetItem *> items = w_users_list->findItems(dest, Qt::MatchStartsWith );

            if (!items.isEmpty())
            {
                const QString dest_user = items.first()->text();
                const QString dest_message = msg_dest.takeLast();
                qDebug() << "USER : " << dest_user << " MSG : " << dest_message;

                Xmpp_client::instance()->sendMessage(dest_user, dest_message);
                linechatMap.value(stacked_index)->clear();
            }
            */
        }
        return;
    }


    qDebug() << "Xmpp_client::sendMessage : " << message << " TO : " << jid;


    this->sendMessage(jid, message);
    //chatroomMap.value(stacked_index)->append("Me : " + message);
    chatroomMap.value(stacked_index)->addMessage("Me", message);
    linechatMap.value(stacked_index)->clear();
}

void Xmpp_client::action_addContact()
{
    bool ok;
    QString bareJid = QInputDialog::getText(new QWidget, "Add a jabber contact",
                                            "Contact ID:", QLineEdit::Normal, "", &ok);

    if(!ok)
        return;

    if(!isValidBareJid(bareJid))
    {
        QMessageBox::information(new QWidget, "Invalid ID", "Specified ID <I>"+bareJid + " </I> is invalid.");
        return;
    }

    if(ok && !bareJid.isEmpty())
    {
        QXmppPresence subscribe;
        subscribe.setTo(bareJid);
        subscribe.setType(QXmppPresence::Subscribe);
        this->sendPacket(subscribe);
    }
}
