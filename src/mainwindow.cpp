/*
 * nodecast using Qt5 and libtorrent.
 * Copyright (C) 2014-2015  Frederic Logier
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
 * Contact : fred@pcode.io
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    #ifndef QT_NO_SYSTEMTRAYICON
        m_trayIcon(this), m_trayIconMenu(this),
    #endif
        m_quitAction("Quit", this),
        ui(new Ui::MainWindow)
{
    bool check;

    ui->setupUi(this);

    /*
    roster = new Ui::Roster;
    QWidget *roster_widget= new QWidget;
    roster->setupUi(roster_widget);
    QVBoxLayout *layoutRoster = new QVBoxLayout;
    layoutRoster->addWidget(&m_statusWidget);
    layoutRoster->addWidget(roster_widget);


    QTextEdit * user_chat = new QTextEdit;

    QSplitter *vRosterSplitter = new QSplitter(Qt::Horizontal, this);

    QGroupBox *groupBoxContact = new QGroupBox;
    groupBoxContact->setLayout(layoutRoster);

    vRosterSplitter->addWidget(groupBoxContact);
    vRosterSplitter->addWidget(user_chat);
    ui->horizontalLayout_contact->addWidget(vRosterSplitter);
*/
/*

    check = connect(&m_statusWidget, SIGNAL(statusTextChanged(QString)),
                    SLOT(statusTextChanged(QString)));
    Q_ASSERT(check);
*/


    createTrayIconAndMenu();
    QFont font;
    font.setBold(true);
    font.setUnderline(true);
    ui->groupBox_media->setFont(font);

    vSplitter = new QSplitter(Qt::Horizontal, this);
    vSplitter->insertWidget(0, ui->groupBox_media);



    //this->setWindowFlags( Qt::WindowMinimizeButtonHint );
#ifdef Q_OS_MAC
   // fixNativeWindow( this );
#endif

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdownCleanUp()), Qt::DirectConnection);

    // hide tchat room
    //ui->groupBox_tchat->hide();
    //ui->pushButton_play->hide();
    ui->widget_media_tools->hide();
    ui->groupBox_media->setTitle("news");
    ui->pushButton_spherenew->setEnabled(false);

    videoPlayer = NULL;

    m_scanFolders = ScanFoldersModel::instance(this);


    //m_godcastapi = new Godcast_api("127.0.0.1", "3000", this);
    //connect(m_godcastapi, SIGNAL(users_Changed(QVariantMap)), this, SLOT(refresh_users(QVariantMap)), Qt::QueuedConnection);
    //connect(m_godcastapi, SIGNAL(spheres_Changed(QVariantMap)), this, SLOT(refresh_spheres(QVariantMap)), Qt::QueuedConnection);
    //connect(m_godcastapi, SIGNAL(nodestatus_Changed(QString)), this, SLOT(node_changed(QString)), Qt::QueuedConnection);

    m_stacked_tab_room = new QStackedWidget;
    m_stacked_tab_room->hide();
    vSplitter->insertWidget(1, m_stacked_tab_room);


    hSplitter = new QSplitter(Qt::Vertical, this);
    transferList = new TransferListWidget(hSplitter, this, QBtSession::instance());
    hSplitter->insertWidget(1, transferList);
    transferList->hide();
    ui->verticalLayout_medias->addWidget(hSplitter);
//    ui->horizontalLayout_media->addWidget(m_stacked_tab_room);
    ui->horizontalLayout_media->addWidget(vSplitter);

    // Resume unfinished torrents
    QBtSession::instance()->startUpTorrents();

    // Populate the transfer list
    transferList->getSourceModel()->populate();

    connect(QBtSession::instance(), SIGNAL(addedTorrent(QTorrentHandle)), this, SLOT(addTorrent(QTorrentHandle)));
    //connect(QBtSession::instance(), SIGNAL(newDownloadedTorrent(QString, QString)), this, SLOT(processDownloadedFiles(QString, QString)));

    connect(QBtSession::instance(), SIGNAL(UPnPSuccess(bool)), this, SLOT(NatChangeConnectionStatus(bool)));



    m_stacked_tab_medias = new QStackedWidget;
  //  ui->verticalLayout_medias->addWidget(m_stacked_tab_medias);


    Sphere_data news;
    news.title =  "news";
    news.scope = Sphere_scope::FIXED;
    news.url = "http://blog.nodecast.net";

    m_spheres_private.insert(news.title,new Sphere(news, m_stacked_tab_room, m_stacked_tab_medias) );
    ui->verticalLayout_sphereprivate->addWidget(m_spheres_private[news.title]);
    connect(m_spheres_private[news.title], SIGNAL(row(int)), this, SLOT(changePage(int)));
    sphere_tab.insert(m_spheres_private[news.title]->index_tab, m_spheres_private[news.title]);


//    Sphere_data halloffame;
//    halloffame.title =  "hall of fame";
//    halloffame.scope = Sphere_scope::FIXED;
//    halloffame.directory = "";
//    m_spheres_public.insert(halloffame.title, new Sphere(halloffame, m_stacked_tab_medias) );
//    ui->verticalLayout_spherepublic->addWidget(m_spheres_public[halloffame.title]);
//    connect(m_spheres_public[halloffame.title], SIGNAL(row(int)), this, SLOT(changePage(int)));
//    sphere_tab.insert( m_spheres_public[halloffame.title]->index_tab, m_spheres_public[halloffame.title]);


    Sphere_data nodecast;
    nodecast.title =  "nodecast";
    nodecast.scope = Sphere_scope::PUBLIC;
    nodecast.url = "http://blog.nodecast.net";
    m_spheres_public.insert(nodecast.title, new Sphere(nodecast, m_stacked_tab_room, m_stacked_tab_medias) );
    ui->verticalLayout_spherepublic->addWidget(m_spheres_public[nodecast.title]);
    connect(m_spheres_public[nodecast.title], SIGNAL(row(int)), this, SLOT(changePage(int)));
    sphere_tab.insert( m_spheres_public[nodecast.title]->index_tab, m_spheres_public[nodecast.title]);
    m_spheres_public[nodecast.title]->populate();



    hSplitter->insertWidget(0, m_stacked_tab_medias);


    load_spheres();
    Xmpp_client::connectXMPP();
    connect(Xmpp_client::instance(), SIGNAL(emit_connected(bool)), this, SLOT(XmppChangeConnectionStatus(bool)));
    connect(Xmpp_client::instance(), SIGNAL(emit_chat(QString, QString)), this, SLOT(receiveMessageChat(QString, QString)));
    connect(Xmpp_client::instance(), SIGNAL(emit_invitation(QString,QString,QString)), this, SLOT(receiveInvitation(QString, QString, QString)));

    connect(Xmpp_client::instance(), SIGNAL(emit_room(QString, QXmppMucRoom*)), this, SLOT(mapRoom(QString, QXmppMucRoom*)));

    connect(Xmpp_client::instance(), SIGNAL(emit_receive_rawfile(QString,QString, QXmppTransferJob*)), this, SLOT(receiveRawFile(QString, QString, QXmppTransferJob*)));




    ui->horizontalLayout_contact->addWidget(Xmpp_client::instance()->getRosterSplitter());
}


void MainWindow::createTrayIconAndMenu()
{
    bool check;
    Q_UNUSED(check);

    check = connect(&m_quitAction, SIGNAL(triggered()), this, SLOT(on_actionQuit_triggered()));
    Q_ASSERT(check);

  //  check = connect(&m_signOutAction, SIGNAL(triggered()), SLOT(action_signOut()));
  //  Q_ASSERT(check);

#ifndef QT_NO_SYSTEMTRAYICON
    m_trayIcon.setIcon(QIcon(":/img/logo/nodecast_logo22.png"));

    check = connect(&m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                    SLOT(action_trayIconActivated(QSystemTrayIcon::ActivationReason)));
    Q_ASSERT(check);

   /* m_trayIconMenu.addAction(&m_signOutAction);
    m_trayIconMenu.addSeparator();
  */
    m_trayIconMenu.addAction(&m_quitAction);

    m_trayIcon.setContextMenu(&m_trayIconMenu);
    m_trayIcon.show();
#endif
}


void MainWindow::action_trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        show();
        break;
    default:
        ;
    }
}

void MainWindow::mapRoom(QString room_name, QXmppMucRoom *room)
{
    qDebug() << "MainWindow::mapRoom : " << room_name;

    //QString sphere_name = room_name.split("_").at(0);
    if (m_spheres_private.contains(room_name))
        m_spheres_private.value(room_name)->connectRoom(room);
    else if (m_spheres_public.contains(room_name))
        m_spheres_public.value(room_name)->connectRoom(room);
}


void MainWindow::receiveInvitation(QString invitation, QString from, QString reason)
{
// INVITATION :  "potes_93c8d9761e1f4c85829f46c354a723c4@conference.nodecast.net"
// REASON :  "you have been invited to join this sphere, from fredix"

    if (!invitation.contains("@")) return;
    QString room_dest = invitation.split("@").at(0);
    if (!room_dest.contains("_")) return;

    QString message = "<B>%1</B> wants to invit you to %2 room, %3";
    int retButton = QMessageBox::question(
            this, "Contact invitation", message.arg(from, room_dest, reason),
            QMessageBox::Yes, QMessageBox::No);

    switch(retButton)
    {
    case QMessageBox::Yes:
        {
        QString sphere_name = room_dest.split("_").at(0);
        if (!m_spheres_private.contains(room_dest))
            create_sphere(sphere_name, room_dest);
        else
            m_spheres_private.value(room_dest)->joinRoom();
        }
        break;
    default:
        break;
    }
}



void MainWindow::receiveRawFile(QString sphere_dest, QString file_path, QXmppTransferJob *job)
{
    qDebug() << "MainWindow::ReceiveRawFile : " << file_path;

    if (m_spheres_private.contains(sphere_dest))
        m_spheres_private.value(sphere_dest)->addFile(file_path, job);

}


void MainWindow::receiveMessageChat(QString from, QString message)
{
//    RECEIVE from :  "test_9de2d7f88aa54f73945989280bbdcb3a@conference.nodecast.net/fredix"  MESSAGE :  "fsd"
//    RECEIVE from :  "test_9de2d7f88aa54f73945989280bbdcb3a@conference.nodecast.net"  MESSAGE :  "fredix@nodecast.net/iMac invited you to the room test_9de2d7f88aa54f73945989280bbdcb3a@conference.nodecast.net (invitation)"

    if (!from.contains("@")) return;
    QString room_dest = from.split("@").at(0);
//    if (!room_dest.contains("_")) return;

    // message from a user's room
    if (from.contains("/"))
    {
        //QString from_login = from.split("/").at(1);
        if (m_spheres_private.contains(room_dest))
            m_spheres_private.value(room_dest)->receive_message(from, message);
        else if (m_spheres_public.contains(room_dest))
            m_spheres_public.value(room_dest)->receive_message(from, message);
    }
}


void MainWindow::XmppChangeConnectionStatus(bool status)
{
    QPixmap pix = status? QPixmap(":/Icons/skin/connected.png") : QPixmap(":/Icons/skin/disconnected.png");
    ui->label_xmpp->setPixmap(pix);

    // connect to chat rooms
    if (status)
    {
        ui->pushButton_spherenew->setEnabled(true);
        QList <QString> keys;

        // PRIVATE SPHERES

        keys = m_spheres_private.keys();
        qDebug() << "KEYS SPHERES PRIVATE : " << keys;

        QHashIterator<QString, Sphere*> sphere_private(m_spheres_private);
        while (sphere_private.hasNext()) {
            sphere_private.next();
            qDebug() << "DIR " << sphere_private.value()->getDirectory();


     //   foreach(QString key, keys)
     //   {
          //  qDebug() << " DIR : " << m_spheres_private[key]->get_directory();
            if (sphere_private.value()->isScopePrivate())
                Xmpp_client::instance()->connectToRoom(sphere_private.value()->getDirectory());
        }



        // PUBLIC SPHERES

        keys = m_spheres_public.keys();
        qDebug() << "KEYS SPHERES PUBLIC : " << keys;

        QHashIterator<QString, Sphere*> sphere_public(m_spheres_public);
        while (sphere_public.hasNext()) {
            sphere_public.next();
            qDebug() << "DIR " << sphere_public.value()->getDirectory();


     //   foreach(QString key, keys)
     //   {
          //  qDebug() << " DIR : " << m_spheres_private[key]->get_directory();
            if (sphere_public.value()->isScopePublic())
                Xmpp_client::instance()->connectToRoom(sphere_public.value()->getDirectory());
        }


    }
    else
    {
        foreach (Sphere *sphere , m_spheres_private)
        {
            sphere->flushRoom();
        }
        foreach (Sphere *sphere , m_spheres_public)
        {
            sphere->flushRoom();
        }

        ui->pushButton_spherenew->setEnabled(false);
    }
}


void MainWindow::NatChangeConnectionStatus(bool status)
{
    if (!prefs.isTrackerEnabled()) status = false;
    QPixmap pix = status? QPixmap(":/Icons/skin/connected.png") : QPixmap(":/Icons/skin/disconnected.png");
    ui->label_tracker->setPixmap(pix);
}



void MainWindow::changePage(int index)
{
    m_stacked_tab_medias->setCurrentIndex(index);
    if (sphere_tab[index]->isScopeFixed())
    {
        ui->groupBox_media->setTitle("news");
        ui->widget_media_tools->hide();
        sphere_tab[index]->reloadWeb();

        m_stacked_tab_room->hide();
    }
    else
    {
        ui->groupBox_media->setTitle(sphere_tab[index]->getTitle());
        ui->widget_media_tools->show();

        m_stacked_tab_room->show();
        int room_index = sphere_tab[index]->getRoomIndex();
        if (room_index != -1)
            m_stacked_tab_room->setCurrentIndex(room_index);
        else m_stacked_tab_room->hide();

    }

    qDebug() << "QStackedWidget index : " << index;
}




void MainWindow::addTorrent(const QTorrentHandle &h)
{
    qDebug() << "MainWindow::addTorrent";
    qDebug() << " PATH TORRENT : " << h.save_path();
    QStringList sphere_path = h.save_path().split("/");
    QString sphere_dir = sphere_path.takeAt(sphere_path.size()-2);
    qDebug() << "SPHERE DIR : " << sphere_dir;

    //QString sphere = sphere_dir.split("_").at(0);
    //qDebug() << "SPHERE : " << sphere;


    if (m_spheres_private.contains(sphere_dir))
    {
        m_stacked_tab_medias->setCurrentIndex(m_spheres_private[sphere_dir]->index_tab);
        m_spheres_private[sphere_dir]->addTorrent(h);
    }

}


void MainWindow::populate()
{
    QDir dir;
    QStringList filters;
    dir.setFilter(QDir::AllEntries|QDir::NoDotAndDotDot);

   // filters << "*.avi" << "*.mp4" << "*.mov" << "*.mkv";
   // dir.setNameFilters(filters);
    qDebug() << "SAVE PATH : " << Preferences().getSavePath();
    dir.setPath(Preferences().getSavePath());
    //    dir.setPath("/Volumes/data/videos/");

    QFileInfoList list = dir.entryInfoList();
    qDebug() << "     Bytes Filename";


    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        qDebug() << qPrintable(QString("%1").arg(fileInfo.fileName()));

    //    list_torrents.insert(i, new Widgettorrent(fileInfo.fileName(), this));

        connect(list_torrents.at(i), SIGNAL(emit_title(QString)), this, SLOT(on_media_doubleClicked(QString)));
       // flowLayout->addWidget(list_torrents.at(i));
       // sphere_tab[m_stacked_tab_medias->currentIndex()]->flowLayout->addWidget(list_torrents.at(i));

      }

//      media_scroll->setWidget(media_container);
//      hSplitter->insertWidget(0, media_scroll);


     // ui->label_counter_medias->setText(QString::number(list_torrents.size()));


  // Load the torrents
//  std::vector<torrent_handle> torrents = QBtSession::instance()->getSession()->get_torrents();

//  std::vector<torrent_handle>::const_iterator it = torrents.begin();
//  std::vector<torrent_handle>::const_iterator itend = torrents.end();
//  for ( ; it != itend; ++it)
//  {
//    const QTorrentHandle h(*it);
//    if (HiddenData::hasData(h.hash()))
//      continue;
//  //  addTorrent(h);
//    //h.filename_at()


//  }
//  // Refresh timer
//  connect(&m_refreshTimer, SIGNAL(timeout()), SLOT(forceModelRefresh()));
//  m_refreshTimer.start(m_refreshInterval);
  // Listen for torrent changes

//      connect(QBtSession::instance(), SIGNAL(addedTorrent(QTorrentHandle)), SLOT(addTorrent(QTorrentHandle)));
//      connect(QBtSession::instance(), SIGNAL(torrentAboutToBeRemoved(QTorrentHandle)), SLOT(handleTorrentAboutToBeRemoved(QTorrentHandle)));
//      connect(QBtSession::instance(), SIGNAL(deletedTorrent(QString)), SLOT(removeTorrent(QString)));
//      connect(QBtSession::instance(), SIGNAL(finishedTorrent(QTorrentHandle)), SLOT(handleFinishedTorrent(QTorrentHandle)));
//      connect(QBtSession::instance(), SIGNAL(metadataReceived(QTorrentHandle)), SLOT(handleTorrentUpdate(QTorrentHandle)));
//      connect(QBtSession::instance(), SIGNAL(resumedTorrent(QTorrentHandle)), SLOT(handleTorrentUpdate(QTorrentHandle)));
//      connect(QBtSession::instance(), SIGNAL(pausedTorrent(QTorrentHandle)), SLOT(handleTorrentUpdate(QTorrentHandle)));
//      connect(QBtSession::instance(), SIGNAL(torrentFinishedChecking(QTorrentHandle)), SLOT(handleTorrentUpdate(QTorrentHandle)));
}



void MainWindow::optionsSaved() {
  loadPreferences();
}



// Load program preferences
void MainWindow::loadPreferences(bool configure_session) {
  QBtSession::instance()->addConsoleMessage(tr("Options were saved successfully."));
  const Preferences pref;
  //const bool newSystrayIntegration = pref.systrayIntegration();
  //actionLock_qBittorrent->setVisible(newSystrayIntegration);
//  if (newSystrayIntegration != (systrayIcon!=0)) {
//    if (newSystrayIntegration) {
//      // create the trayicon
//      if (!QSystemTrayIcon::isSystemTrayAvailable()) {
//        if (!configure_session) { // Program startup
//          systrayCreator = new QTimer(this);
//          connect(systrayCreator, SIGNAL(timeout()), this, SLOT(createSystrayDelayed()));
//          systrayCreator->setSingleShot(true);
//          systrayCreator->start(2000);
//          qDebug("Info: System tray is unavailable, trying again later.");
//        }  else {
//          qDebug("Warning: System tray is unavailable.");
//        }
//      } else {
//        createTrayIcon();
//      }
//    } else {
//      // Destroy trayicon
//      delete systrayIcon;
//      delete myTrayIconMenu;
//    }
//  }
  // Reload systray icon
//  if (newSystrayIntegration && systrayIcon) {
//    systrayIcon->setIcon(getSystrayIcon());
//  }
  // General
//  if (pref.isToolbarDisplayed()) {
//    toolBar->setVisible(true);
//  } else {
//    // Clear search filter before hiding the top toolbar
//    search_filter->clear();
//    toolBar->setVisible(false);
//  }

//  if (pref.preventFromSuspend())
//  {
//    preventTimer->start(PREVENT_SUSPEND_INTERVAL);
//  }
//  else
//  {
//    preventTimer->stop();
//    m_pwr->setActivityState(false);
//  }

//  const uint new_refreshInterval = pref.getRefreshInterval();
//  transferList->setRefreshInterval(new_refreshInterval);
//  transferList->setAlternatingRowColors(pref.useAlternatingRowColors());
////  properties->getFilesList()->setAlternatingRowColors(pref.useAlternatingRowColors());
////  properties->getTrackerList()->setAlternatingRowColors(pref.useAlternatingRowColors());
////  properties->getPeerList()->setAlternatingRowColors(pref.useAlternatingRowColors());
//  // Queueing System
//  if (pref.isQueueingSystemEnabled()) {
//    if (!actionDecreasePriority->isVisible()) {
//      transferList->hidePriorityColumn(false);
//      actionDecreasePriority->setVisible(true);
//      actionIncreasePriority->setVisible(true);
//      prioSeparator->setVisible(true);
//      prioSeparatorMenu->setVisible(true);
//    }
//  } else {
//    if (actionDecreasePriority->isVisible()) {
//      transferList->hidePriorityColumn(true);
//      actionDecreasePriority->setVisible(false);
//      actionIncreasePriority->setVisible(false);
//      prioSeparator->setVisible(false);
//      prioSeparatorMenu->setVisible(false);
//    }
//  }

  // Torrent properties
//  properties->reloadPreferences();

  // Icon provider
#if defined(Q_WS_X11)
  IconProvider::instance()->useSystemIconTheme(pref.useSystemIconTheme());
#endif

  if (configure_session)
    QBtSession::instance()->configureSession();

#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
  if (pref.isUpdateCheckEnabled())
    checkProgramUpdate();
  else
    programUpdateTimer.stop();
#endif

  qDebug("GUI settings loaded");
}


void MainWindow::launch_timer_handle(QTorrentHandle h)
{
    qDebug() << "RECEIVE HANDLER";
    m_torrent = h;
    timer_get_torrent_progress = new QTimer(this);
    connect(timer_get_torrent_progress, SIGNAL(timeout()), this, SLOT(update_timer_torrent_progress()));
    timer_get_torrent_progress->start(1000);
}

void MainWindow::shutdownCleanUp()
{
    qDebug("DESTRUCTEUR");
/*
    if (videoPlayer != 0)
    {
        //qDebug() << "process ID = " << videoPlayer->processId();
        videoPlayer->kill();
        delete(videoPlayer);
    }
*/

    //delete ui;
    qDebug() << "stop torrent";
    QBtSession::drop();
    Preferences().sync();

    delete transferList;
    //delete transferListFilters;

    //list_torrents.clear();

    //m_godcastapi->deleteLater();
    //qDebug() << "godcast_api deleted";

    foreach (Sphere *sphere, m_spheres_private)
    {
        delete sphere;
    }

    foreach (Sphere *sphere, m_spheres_public)
    {
        delete sphere;
    }


    Xmpp_client::drop();
    qDebug() << "xmpp deleted";
    qDebug() << "exit app";
    qApp->quit();
}

void MainWindow::on_actionQuit_triggered()
{
    qDebug() << "MainWindow::on_actionQuit_triggered()";
    qApp->quit();
}

//void MainWindow::writeSettings() {
//  QIniSettings settings;
//  settings.beginGroup(QString::fromUtf8("MainWindow"));
//  settings.setValue("geometry", saveGeometry());
  // Splitter size
  //settings.setValue(QString::fromUtf8("vsplitterState"), vSplitter->saveState());
//  settings.endGroup();
//  properties->saveSettings();
//}

//void MainWindow::readSettings() {
//  QIniSettings settings;
//  settings.beginGroup(QString::fromUtf8("MainWindow"));
//  if (settings.contains("geometry")) {
//    if (restoreGeometry(settings.value("geometry").toByteArray()))
//      m_posInitialized = true;
// }
 // const QByteArray splitterState = settings.value("vsplitterState").toByteArray();
 // if (splitterState.isEmpty()) {
    // Default sizes
 //   vSplitter->setSizes(QList<int>() << 120 << vSplitter->width()-120);
 // } else {
 //   vSplitter->restoreState(splitterState);
 // }
//  settings.endGroup();
//}




void MainWindow::processDownloadedFiles(QString path, QString url) {
  Preferences pref;
 // if (pref.useAdditionDialog())
    //AddNewTorrentDialog::showTorrent(path, url, this);
//  else
    QBtSession::instance()->addTorrent(path, false, url);
}


void MainWindow::downloadFromURLList(const QStringList& url_list) {
  Preferences pref;
  const bool useTorrentAdditionDialog = pref.useAdditionDialog();
  foreach (QString url, url_list) {
    if (url.startsWith("bc://bt/", Qt::CaseInsensitive)) {
      qDebug("Converting bc link to magnet link");
      url = misc::bcLinkToMagnet(url);
    }
    if ((url.size() == 40 && !url.contains(QRegExp("[^0-9A-Fa-f]")))
        || (url.size() == 32 && !url.contains(QRegExp("[^2-7A-Za-z]")))) {
      url = "magnet:?xt=urn:btih:" + url;
    }
    if (url.startsWith("magnet:", Qt::CaseInsensitive)) {
    //  if (useTorrentAdditionDialog)
    //    AddNewTorrentDialog::showMagnet(url, this);
    //  else
        QBtSession::instance()->addMagnetUri(url);
    }
    else if (url.startsWith("http://", Qt::CaseInsensitive) || url.startsWith("https://", Qt::CaseInsensitive)
             || url.startsWith("ftp://", Qt::CaseInsensitive)) {
      QBtSession::instance()->downloadFromUrl(url);
    }
  }
}




void MainWindow::update_timer_torrent_progress()
{
//    if (torrent_index == 0) return;

//    int progress = 0;
//    int havePieces = 0;
//    int pieceBytes = 0;
//    QByteArray url;
//    url.resize( 1000 );
//    bool haveUrl = false;

//    dlInfo stats = torrent->getTorrentInfo(torrent_index);


/*    if (stats.downloaded == stats.total)
    {
        timer_get_torrent_progress->stop();
        return;
    }
*/

//    pieceBytes += stats.downloadRateBs / TORRENT_INFO_UPDATES_PER_SECOND;


//    const int currentProgress = ( pieceBytes + stats.piecesDone * stats.pieceSize ) * 100 / ( stats.pieceSize * TORRENT_PIECES_TO_PLAY );
//    if ( currentProgress > progress ) progress = currentProgress;
//    if ( progress > 100 ) progress = 100;
//    else if ( progress == 0 && (stats.seeders || stats.peers ) ) progress = 3;



    qDebug() << "PROGRESS : " << m_torrent.progress();;
    float toto = m_torrent.progress();
    qDebug() << "PROGRESS INT : " << toto;
    float tmp = m_torrent.progress() * 100;
    qDebug() << "PROGRESS float * 100 : " << tmp;

    int progress = int(tmp + 0.5);
    qDebug() << "PROGRESS int * 100 : " << progress;


//    int pourcentage = (int) (stats.downloaded*100) / stats.total;

//    qDebug() << "torrent_index : " << torrent_index;
//    qDebug() << "TIMER STATS SEEDERS : " << stats.seeders;
//    qDebug() << "TIMER STATS PEERS : " << stats.peers;
//    qDebug() << "TIMER STATS DOWNLOADED : " << stats.downloaded;
//    qDebug() << "TIMER STATS DOWNLOADED pourcentage : " << pourcentage;
//    qDebug() << "TIMER STATS DOWNLOADED RATE Bs : " << stats.downloadRateBs;
//    qDebug() << "TIMER STATS TOTAL : " << stats.total;


    ui->progressBar_torrent->setValue(progress);

    if (progress == 100)
    {
        timer_get_torrent_progress->stop();
        return;
    }

}


void MainWindow::on_actionPreferences_triggered()
{
    //tprefs.show();


    // Display Program Options
      if (options) {
        // Get focus
        options->setFocus();
      } else {
        options = new options_imp(this);
        connect(options, SIGNAL(status_changed()), this, SLOT(optionsSaved()));
      }



}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "LAUNCH PLAYER";

#if defined (Q_OS_MAC)
    QString program = "open";
    QStringList arguments;
    arguments << "-a" << settings.value("player").toString() << settings.value("data").toString() + "/Too.Big.To.Fail.2011.FRENCH.DVDRiP.XViD-STVFRV.avi";
#else
    QString program = settings.value("player").toString();
    QStringList arguments;
    arguments << settings.value("data").toString() + "/Too.Big.To.Fail.2011.FRENCH.DVDRiP.XViD-STVFRV.avi";
#endif

    videoPlayer = new QProcess(this);
    videoPlayer->start(program, arguments);
}



void MainWindow::on_media_doubleClicked(QString video)
{
    qDebug() << "LAUNCH PLAYER : ";

    QString program;
    QStringList arguments;

#if defined (Q_OS_MAC)
    program = "open";
    arguments << "-a" << Preferences().getVideoPlayer() << Preferences().getSavePath() + "/" + video;
#else
    program = settings.value("player").toString();
    arguments << settings.value("data").toString() + video;
#endif

    qDebug() << "program : " << program;

    videoPlayer = new QProcess(this);
    videoPlayer->start(program, arguments);
}


void MainWindow::on_listWidget_media_doubleClicked(const QModelIndex &index)
{
    qDebug() << "LAUNCH PLAYER : ";

//    QString video = ui->listWidget_media->item(index.row())->text();
//    qDebug() << "VIDEO : " << video;

//    QString program;
//    QStringList arguments;

//#if defined (Q_OS_MAC)
//    program = "open";
//    arguments << "-a" << settings.value("player").toString() << settings.value("data").toString() + "/" + video;
//#else
//    program = settings.value("player").toString();
//    arguments << settings.value("data").toString() + video;
//#endif

//    qDebug() << "program : " << program;

//    videoPlayer = new QProcess(this);
//    videoPlayer->start(program, arguments);
}

void MainWindow::on_pushButton_play_clicked()
{
    qDebug() << "LAUNCH PLAYER : ";

//    QList <QListWidgetItem*> item_list = ui->listWidget_media->selectedItems();
//    if (item_list.length() == 0) return;

//    QString video = item_list.at(0)->text();
//    qDebug() << "VIDEO : " << video;

//    QString program;
//    QStringList arguments;

//#if defined (Q_OS_MAC)
//    program = "open";
//    arguments << "-a" << settings.value("player").toString() << settings.value("data").toString() + "/" + video;
//#else
//    program = settings.value("player").toString();
//    arguments << settings.value("data").toString() + video;
//#endif

//    qDebug() << "program : " << program;

//    videoPlayer = new QProcess(this);
//    videoPlayer->start(program, arguments);
}


void MainWindow::on_pushButton_spherenew_clicked()
{

    if (ui->tabWidget_spheres->currentIndex() == 0)
    {

        qDebug() << "NEW SPEHRE";
        new_sphere = new newsphere(this);
        //newsphere new_sphere;
        connect(new_sphere, SIGNAL(create_sphere(QString)), SLOT(create_sphere(QString)));
        new_sphere->show();;

        //Sphere *sphere = new Sphere("private", QColor(105,105,105), this);
        //ui->verticalLayout_sphereprivate->addWidget(sphere);
    }
    else
    {

        Sphere_data spublic;
        spublic.title =  "public";
        spublic.scope = Sphere_scope::PUBLIC;

        Sphere *sphere = new Sphere(spublic, m_stacked_tab_medias, m_stacked_tab_medias);
        ui->verticalLayout_spherepublic->addWidget(sphere);
    }

}


void MainWindow::load_spheres()
{
    qDebug() << "LOAD SHERES";
    QStringList spheres;

    QDir path(prefs.getSavePath() + "/nodecast/spheres/private");
    if (path.exists())
    {
        spheres = path.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
        qDebug() << "SPHERES DIRECTORY : " << spheres;

        foreach(QString sphere_dir, spheres)
        {
            qDebug() << "SPHERE DIR : " << sphere_dir;

            QString sphere_name = sphere_dir.split("_").at(0);

            Sphere_data sphere_datas;
            sphere_datas.title =  sphere_name;
            sphere_datas.directory = sphere_dir;
            sphere_datas.scope = Sphere_scope::PRIVATE;
            m_spheres_private.insert(sphere_dir, new Sphere(sphere_datas, m_stacked_tab_room, m_stacked_tab_medias) );
            ui->verticalLayout_sphereprivate->addWidget(m_spheres_private[sphere_dir]);
            connect(m_spheres_private[sphere_dir], SIGNAL(row(int)), this, SLOT(changePage(int)));
            connect(m_spheres_private[sphere_dir], SIGNAL(emit_deleted(Sphere*)), this, SLOT(delete_sphere(Sphere*)));

            sphere_tab.insert(m_spheres_private[sphere_dir]->index_tab, m_spheres_private[sphere_dir]);
            m_spheres_private[sphere_dir]->populate();
        }
    }
}





void MainWindow::create_sphere(QString sphere_name)
{
    qDebug() << "CREATE SHERE : " << sphere_name;
    if (sphere_name.isEmpty()) return;

    QString sphere_dir = Sphere::gen_directory(sphere_name);


//    QDir nodecast_datas(prefs.getSavePath() + "/nodecast/spheres/private/");
//    QDir check_dir(nodecast_datas.absolutePath() + "/" + sphere_name);
  //  if (!m_spheres_private.contains(sphere_name))
  //  {
 //       nodecast_datas.mkdir(sphere_name);

        Sphere_data sphere_datas;
        sphere_datas.title =  sphere_name;
        sphere_datas.directory =  sphere_dir;
        sphere_datas.scope = Sphere_scope::PRIVATE;
        m_spheres_private.insert(sphere_dir, new Sphere(sphere_datas, m_stacked_tab_room, m_stacked_tab_medias) );
        ui->verticalLayout_sphereprivate->addWidget(m_spheres_private[sphere_dir]);
        connect(m_spheres_private[sphere_dir], SIGNAL(row(int)), this, SLOT(changePage(int)));
        connect(m_spheres_private[sphere_dir], SIGNAL(emit_deleted(Sphere*)), this, SLOT(delete_sphere(Sphere*)));

        sphere_tab.insert(m_spheres_private[sphere_dir]->index_tab, m_spheres_private[sphere_dir]);

        //QString sphere_dir = m_spheres_private[sphere_name]->getDirectory();
        Xmpp_client::instance()->connectToRoom(sphere_dir);
   // }

    //QString target_link = prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_data.title + "/" + fileInfo.fileName();


    //QDir::mkdir(prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_name);

    //m_godcastapi->sphere_post("okok", sphere_name);
    //m_godcastapi nodeapi->workflow_post(ui->lineEdit_token->text(), workflow_name, owner_email, json);

}

void MainWindow::create_sphere(QString sphere_name, QString sphere_dir)
{
    qDebug() << "CREATE SHERE FROM INVITATION : " << sphere_name << " DIR : " << sphere_dir;
    if (sphere_name.isEmpty()) return;


//    QDir nodecast_datas(prefs.getSavePath() + "/nodecast/spheres/private/");
//    QDir check_dir(nodecast_datas.absolutePath() + "/" + sphere_name);
  //  if (!m_spheres_private.contains(sphere_name))
  //  {
 //       nodecast_datas.mkdir(sphere_name);

        Sphere_data sphere_datas;
        sphere_datas.title =  sphere_name;
        sphere_datas.directory =  sphere_dir;
        sphere_datas.scope = Sphere_scope::PRIVATE;
        m_spheres_private.insert(sphere_dir, new Sphere(sphere_datas, m_stacked_tab_room, m_stacked_tab_medias) );
        ui->verticalLayout_sphereprivate->addWidget(m_spheres_private[sphere_dir]);
        connect(m_spheres_private[sphere_dir], SIGNAL(row(int)), this, SLOT(changePage(int)));
        connect(m_spheres_private[sphere_dir], SIGNAL(emit_deleted(Sphere*)), this, SLOT(delete_sphere(Sphere*)));

        sphere_tab.insert(m_spheres_private[sphere_dir]->index_tab, m_spheres_private[sphere_dir]);

        //QString sphere_dir = m_spheres_private[sphere_name]->getDirectory();
        Xmpp_client::instance()->connectToRoom(sphere_dir);
   // }

    //QString target_link = prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_data.title + "/" + fileInfo.fileName();


    //QDir::mkdir(prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_name);

    //m_godcastapi->sphere_post("okok", sphere_name);
    //m_godcastapi nodeapi->workflow_post(ui->lineEdit_token->text(), workflow_name, owner_email, json);

}





void MainWindow::delete_sphere(Sphere *sphere)
{        
    QString sphere_dir = sphere->getDirectory();

    if (m_spheres_private.contains(sphere_dir))
    {
        qDebug() << "DELETE SHERE : " << sphere->getTitle() << " DIR : " << sphere_dir;

        m_stacked_tab_room->removeWidget(sphere->getRoomWidget());
        m_stacked_tab_medias->removeWidget(sphere);

        ui->verticalLayout_sphereprivate->removeWidget(m_spheres_private.value(sphere_dir));
        sphere_tab.remove(sphere->index_tab);
        m_spheres_private.remove(sphere_dir);
        sphere->hide();
        sphere->deleteLater();
    }
}


void MainWindow::on_actionAccount_triggered()
{      
    accountDlg = new account(this);
}



// Display About Dialog
void MainWindow::on_actionAbout_triggered() {
  //About dialog
  if (aboutDlg) {
    aboutDlg->setFocus();
  } else {
    aboutDlg = new about(this);
  }
}




void MainWindow::refresh_spheres(QVariantMap sphere)
{
    qDebug() << "REFRESH SPHERES !!";
    qDebug() << "M SPHERENAME  : " << sphere["spherename"].toString();
    qDebug() << "M UUID  : " << sphere["sphere_uuid"].toString();

    QStringList row;
    row << sphere["id"].toString() << sphere["spherename"].toString() << sphere["sphere_uuid"].toString() << "test";

//    m_spheres_private.append( new Sphere(sphere["spherename"].toString(), QColor(192,192,192), this));


    Sphere_data sprivate;
    sprivate.title = sphere["spherename"].toString();
    sprivate.scope = Sphere_scope::PRIVATE;

    m_spheres_private.insert(sprivate.title, new Sphere(sprivate, m_stacked_tab_room, m_stacked_tab_medias));
    ui->verticalLayout_sphereprivate->addWidget(m_spheres_private[sprivate.title]);




  //  QTreeWidgetItem *pRow = new QTreeWidgetItem(row);
    // ui->treeWidget_nodes->addTopLevelItem(pRow);
}


void MainWindow::on_actionTransferts_triggered()
{
    transferList->isHidden()? transferList->show() : transferList->hide();
    qDebug() << "SHOW TRANSFERT";
}

void MainWindow::on_actionXml_console_triggered()
{
    Xmpp_client::instance()->show_xml_console();
}

