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
    ui->setupUi(this);
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
    ui->pushButton_torrent_info->hide();
    ui->widget_media_tools->hide();
    ui->groupBox_media->setTitle("news");
    ui->pushButton_spherenew->setEnabled(false);

    videoPlayer = NULL;


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
    news.url = "http://nodecast.wordpress.com";

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


    Sphere_data debian;
    debian.title =  "debian";
    debian.scope = Sphere_scope::PUBLIC;
    debian.url = "http://debian.org";
    debian.directory = "";
    m_spheres_public.insert(debian.title, new Sphere(debian, m_stacked_tab_room, m_stacked_tab_medias) );
    ui->verticalLayout_spherepublic->addWidget(m_spheres_public[debian.title]);
    connect(m_spheres_public[debian.title], SIGNAL(row(int)), this, SLOT(changePage(int)));
    sphere_tab.insert( m_spheres_public[debian.title]->index_tab, m_spheres_public[debian.title]);

    //m_spheres_public.last()->populate();


    hSplitter->insertWidget(0, m_stacked_tab_medias);


    load_spheres();
    Xmpp_client::connectXMPP();
    connect(Xmpp_client::instance(), SIGNAL(emit_connected(bool)), this, SLOT(XmppChangeConnectionStatus(bool)));
    connect(Xmpp_client::instance(), SIGNAL(emit_chat(QString, QString)), this, SLOT(receiveMessageChat(QString, QString)));
    connect(Xmpp_client::instance(), SIGNAL(emit_room(QString, QXmppMucRoom*)), this, SLOT(mapRoom(QString, QXmppMucRoom*)));

}


void MainWindow::createTrayIconAndMenu()
{
    bool check;
    Q_UNUSED(check);

    check = connect(&m_quitAction, SIGNAL(triggered()), SLOT(shutdownCleanUp()));
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

    QString sphere_name = room_name.split("_").at(0);
    if (m_spheres_private.contains(sphere_name))
        m_spheres_private.value(sphere_name)->connectRoom(room);
}


void MainWindow::receiveMessageChat(QString from, QString message)
{
//    RECEIVE from :  "test_9de2d7f88aa54f73945989280bbdcb3a@conference.nodecast.net/fredix"  MESSAGE :  "fsd"
//    RECEIVE from :  "test_9de2d7f88aa54f73945989280bbdcb3a@conference.nodecast.net"  MESSAGE :  "fredix@nodecast.net/iMac invited you to the room test_9de2d7f88aa54f73945989280bbdcb3a@conference.nodecast.net (invitation)"

    if (!from.contains("@")) return;
    QString room_dest = from.split("@").at(0);
    if (!room_dest.contains("_")) return;

    // message from a user's room
    if (from.contains("/"))
    {
        QString from_login = from.split("/").at(1);
        QString sphere_name = room_dest.split("_").at(0);
        if (m_spheres_private.contains(sphere_name))
            m_spheres_private.value(sphere_name)->receive_message(from_login + " : " + message);
    }
    else
    {
        // message from a room like an invitation
        qDebug() << "INVITATION FROM : " << from << " MESSAGE : " << message;
        return;
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


        QList <QString> keys = m_spheres_private.keys();
        qDebug() << "KEYS : " << keys;

        QHashIterator<QString, Sphere*> sphere(m_spheres_private);
        while (sphere.hasNext()) {
            sphere.next();
            qDebug() << "DIR " << sphere.value()->getDirectory();


     //   foreach(QString key, keys)
     //   {
          //  qDebug() << " DIR : " << m_spheres_private[key]->get_directory();
            if (sphere.value()->isScopePrivate())
                Xmpp_client::instance()->connectToRoom(sphere.value()->getDirectory());
        }
    }
    else
        ui->pushButton_spherenew->setEnabled(false);

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

 //   h.filename_at()
 //   list_torrents.insert(i, new Widgettorrent(fileInfo.fileName(), this));

//    qDebug() << "HASH LABEL : " <<  TorrentPersistentData::getLabel(h.hash());

//    qDebug() << "HASH NAME : " <<  TorrentPersistentData::getName(h.hash());
//    qDebug() << "HASH NAME2 : " << h.name();


//    qDebug() << "HASH : " << misc::toQString(h.info_hash());

//    int size = list_torrents.size();

//    list_torrents.append(new Widgettorrent());

//    flowLayout->addWidget(list_torrents.at(size));

    qDebug() << "MainWindow::addTorrent";
    qDebug() << " PATH TORRENT : " << h.save_path();
    QStringList sphere_path = h.save_path().split("/");
    QString sphere_dir = sphere_path.takeAt(sphere_path.size()-2);
    qDebug() << "SPHERE DIR : " << sphere_dir;

    QString sphere = sphere_dir.split("_").at(0);
    qDebug() << "SPHERE : " << sphere;


    m_stacked_tab_medias->setCurrentIndex(m_spheres_private[sphere]->index_tab);
    m_spheres_private[sphere]->addTorrent(h);




//    Widgettorrent *wt = new Widgettorrent();
//    wt->addTorrent(h);
    // force stacked to sphere test
//    m_stacked_tab_medias->setCurrentIndex(1);
    //flowLayout->addWidget(wt);

  //  sphere_tab[m_stacked_tab_medias->currentIndex()]->flowLayout->addWidget(wt);


 //   sphere_tab[m_stacked_tab_medias->currentIndex()]->addTorrent(h);


    //ui->label_counter_medias->setText(QString::number(list_torrents.size()));
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
 //   if (torrent)
 //   {
 //       torrent->deleteLater();
 //       thread_torrent->wait();
 //   }

    delete transferList;
    //delete transferListFilters;

    //list_torrents.clear();

    //m_godcastapi->deleteLater();
    //qDebug() << "godcast_api deleted";

    foreach (Sphere *sphere, m_spheres_private)
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
 //   if (torrent)
 //   {
 //       torrent->deleteLater();
 //       thread_torrent->wait();
 //   }
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



void MainWindow::on_actionOpen_triggered()
{
    // open TORRENT

    qDebug("OPEN");


    if (!downloadFromURLDialog) {
      downloadFromURLDialog = new downloadFromURL(this);
      connect(downloadFromURLDialog, SIGNAL(urlsReadyToBeDownloaded(QStringList)), this, SLOT(downloadFromURLList(QStringList)));
    }
    return;

    //openuritorrent.show();
    //return;


 //   QString link = "magnet:?xt=urn:btih:03de3df2c7339a90926606afb8f8666cd21db06d&dn=The.Anomaly.2014.DVDRip.XviD-EVO&tr=udp%3A%2F%2Ftracker.openbittorrent.com%3A80&tr=udp%3A%2F%2Ftracker.publicbt.com%3A80&tr=udp%3A%2F%2Ftracker.istole.it%3A6969&tr=udp%3A%2F%2Fopen.demonii.com%3A1337";
 //   QBtSession::instance()->addMagnetUri(link);


//    connect(QBtSession::instance(), SIGNAL(fullDiskError(QTorrentHandle, QString)), this, SLOT(fullDiskError(QTorrentHandle, QString)));
//    connect(QBtSession::instance(), SIGNAL(finishedTorrent(QTorrentHandle)), this, SLOT(finishedTorrent(QTorrentHandle)));
//    connect(QBtSession::instance(), SIGNAL(trackerAuthenticationRequired(QTorrentHandle)), this, SLOT(trackerAuthenticationRequired(QTorrentHandle)));
//    connect(QBtSession::instance(), SIGNAL(newDownloadedTorrent(QString, QString)), this, SLOT(processDownloadedFiles(QString, QString)));
//    connect(QBtSession::instance(), SIGNAL(newMagnetLink(QString)), this, SLOT(processNewMagnetLink(QString)));
//    connect(QBtSession::instance(), SIGNAL(downloadFromUrlFailure(QString, QString)), this, SLOT(handleDownloadFromUrlFailure(QString, QString)));
//    connect(QBtSession::instance(), SIGNAL(alternativeSpeedsModeChanged(bool)), this, SLOT(updateAltSpeedsBtn(bool)));
//    connect(QBtSession::instance(), SIGNAL(recursiveTorrentDownloadPossible(QTorrentHandle)), this, SLOT(askRecursiveTorrentDownloadConfirmation(QTorrentHandle)));




    /*
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "torrent",
                                                     tr("Files (*.torrent)"));
    */

//    thread_torrent = new QThread(this);
//    torrent = new Torrent();

//    this->connect(thread_torrent, SIGNAL(started()), torrent, SLOT(init()));
//    this->connect(torrent, SIGNAL(destroyed()), thread_torrent, SLOT(quit()), Qt::DirectConnection);

//    torrent->moveToThread(thread_torrent);
//    thread_torrent->start();


    //torrent->get_torrent(fileName);

  //  QString torrent_file ="http://www.freetorrent.fr/download.php?id=b4652d4202e4abe57e82411ec32b39bb127da328&f=Framakey+Ubuntu+Remix+12.04.torrent";

    //QString torrent_file = "http://www.freetorrent.fr/download.php?id=07b789accb342ae9c7761e03998f10eaa8ca44aa&f=Copier+n%27est+pas+voler+720p+stfr.torrent";

    //QString torrent_file = "http://cdimage.debian.org/debian-cd/current-live/amd64/bt-hybrid/debian-live-7.6.0-amd64-kde-desktop.iso.torrent";


    //QString torrent_file = "http://www.freetorrent.fr/download.php?id=41dc622ea17716a140dbdf8a0b5c445886764e0d&f=Vid%C3%A9os+mp4+Capitole+du+libre+2013.torrent";


    //QString torrent_file = "http://download.blender.org/peach/bigbuckbunny_production.torrent";


    //QString torrent_file = "magnet:?xt=urn:btih:2ff42e31f1014987e3fbbf7c7bc5544d0ee833d3&dn=Teenage.Mutant.Ninja.Turtles.2014.HDRip.XviD.MP3-RARBG&tr=udp%3A%2F%2Ftracker.openbittorrent.com%3A80&tr=udp%3A%2F%2Ftracker.publicbt.com%3A80&tr=udp%3A%2F%2Ftracker.istole.it%3A6969&tr=udp%3A%2F%2Fopen.demonii.com%3A1337";


    //QString torrent_file = "magnet:?xt=urn:btih:03de3df2c7339a90926606afb8f8666cd21db06d&dn=The.Anomaly.2014.DVDRip.XviD-EVO&tr=udp%3A%2F%2Ftracker.openbittorrent.com%3A80&tr=udp%3A%2F%2Ftracker.publicbt.com%3A80&tr=udp%3A%2F%2Ftracker.istole.it%3A6969&tr=udp%3A%2F%2Fopen.demonii.com%3A1337";
//    torrent_index = torrent->downloadTorrent(torrent_file, "The.Anomaly.2014.DVDRip.XviD-EVO");
    //torrent_index = torrent->downloadTorrent(torrent_file, "PLOP");



    //qDebug() << "RES torrent : " << torrent_index;




//    dlInfo stats = torrent->getTorrentInfo(torrent_index);

//    ui->progressBar_torrent->setMaximum(100);
//    ui->progressBar_torrent->setMinimum(0);

//    qDebug() << "STATS PEERS : " << stats.peers;
//    qDebug() << "STATS DOWNLOADED : " << stats.downloaded;
//    qDebug() << "STATS TOTAL : " << stats.total;


    //update_timer_torrent_progress();

    //qDebug() << "TORRENT : " << fileName;

/*
    QString path = QFileDialog::getExistingDirectory (this, tr("Directory"), directory.path());
    if ( path.isNull() == false )
    {
        directory.setPath(path);
    }

*/


/*    QTextEdit textEdit;
      QPushButton quitButton("Quit");

      //QObject::connect(&quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

      QVBoxLayout layout;
      layout.addWidget(&textEdit);
      layout.addWidget(&quitButton);

      QWidget window;
      window.setLayout(&layout);

      window.show();*/

//    Video video();
//    video.show();

}

//void MainWindow::processNewMagnetLink(const QString& link) {
//  Preferences pref;
//  if (pref.useAdditionDialog())
//    AddNewTorrentDialog::showMagnet(link, this);
//  else
//    QBtSession::instance()->addMagnetUri(link);
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

void MainWindow::on_pushButton_torrent_info_clicked()
{


    dlInfo stats = torrent->getTorrentInfo(torrent_index);

    qDebug() << "STATS PEERS : " << stats.peers;
    qDebug() << "STATS SEEDERS : " << stats.seeders;

    qDebug() << "STATS DOWNLOADED : " << stats.downloaded;
    qDebug() << "STATS TOTAL : " << stats.total;
    qDebug() << "STATS downloadRateBs : " << stats.downloadRateBs;




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
            m_spheres_private.insert(sphere_name, new Sphere(sphere_datas, m_stacked_tab_room, m_stacked_tab_medias) );
            ui->verticalLayout_sphereprivate->addWidget(m_spheres_private[sphere_name]);
            connect(m_spheres_private[sphere_name], SIGNAL(row(int)), this, SLOT(changePage(int)));

            sphere_tab.insert(m_spheres_private[sphere_name]->index_tab, m_spheres_private[sphere_name]);
            m_spheres_private[sphere_name]->populate();
        }
    }
}





void MainWindow::create_sphere(QString sphere_name)
{
    qDebug() << "CREATE SHERE : " << sphere_name;


//    QDir nodecast_datas(prefs.getSavePath() + "/nodecast/spheres/private/");
//    QDir check_dir(nodecast_datas.absolutePath() + "/" + sphere_name);
    if (!m_spheres_private.contains(sphere_name))
    {
 //       nodecast_datas.mkdir(sphere_name);

        Sphere_data sphere_datas;
        sphere_datas.title =  sphere_name;
        sphere_datas.scope = Sphere_scope::PRIVATE;
        m_spheres_private.insert(sphere_name, new Sphere(sphere_datas, m_stacked_tab_room, m_stacked_tab_medias) );
        ui->verticalLayout_sphereprivate->addWidget(m_spheres_private[sphere_name]);
        connect(m_spheres_private[sphere_name], SIGNAL(row(int)), this, SLOT(changePage(int)));

        sphere_tab.insert(m_spheres_private[sphere_name]->index_tab, m_spheres_private[sphere_name]);

        QString sphere_dir = m_spheres_private[sphere_name]->getDirectory();
        Xmpp_client::instance()->connectToRoom(sphere_dir);
    }

    //QString target_link = prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_data.title + "/" + fileInfo.fileName();


    //QDir::mkdir(prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_name);






    //m_godcastapi->sphere_post("okok", sphere_name);
    //m_godcastapi nodeapi->workflow_post(ui->lineEdit_token->text(), workflow_name, owner_email, json);

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
