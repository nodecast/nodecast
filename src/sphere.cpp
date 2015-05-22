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


#include "sphere.h"

int Sphere::index = 0;


QString Sphere::gen_directory(QString sphere_name)
{
    QString uuid = QUuid::createUuid().toString().remove("-").replace(0, 1, "");
    uuid = uuid.replace(uuid.size()-1, 1, "");
    return sphere_name.toLower().trimmed() + "_" + uuid;
}


Sphere::Sphere(Sphere_data data, QStackedWidget *stacked_room, QStackedWidget *parent)
        :  m_scanFolders(ScanFoldersModel::instance(this)), m_stacked_room(stacked_room), sphere_data(data), QAbstractButton(parent)
{

//    QPalette pal = palette();
//    pal.setColor(QPalette::Background, Qt::black);
//    setPalette(pal);
//    setAutoFillBackground(true);

  //  data.title.isEmpty()? m_title = data.title : m_title = "";

   // content = new QWidget(parent);

    nb_torrent = 0;
    m_room = NULL;

    qDebug() << "NEW SPHERE DIRECTORY : " << sphere_data.directory;

    QString uuid;
    QDir nodecast_datas;
    QDir check_dir;

    widget_container = new QWidget;
    vbox = new QVBoxLayout(widget_container);
    widget = new QWidget;
    hbox = new QHBoxLayout(widget);


    filter_label = new QLabel;
    filter_label->setText("Filter : ");

    media_label = new QLabel;
    media_label->setText("medias : ");
    media_label_counter = new QLabel;
    media_label_counter->setText("0");

    upload_label = new QLabel;
    upload_label->setText("upload : ");
    upload_label_counter = new QLabel;
    upload_label_counter->setText("0");

    download_label = new QLabel;
    download_label->setText("download : ");
    download_label_counter = new QLabel;
    download_label_counter->setText("0");


    toolButton_video = new QToolButton;
    toolButton_video->setIcon(QIcon(":/img/32x32/movies.png"));
    toolButton_video->setToolTip("video");

    toolButton_image = new QToolButton;
    toolButton_image->setIcon(QIcon(":/img/32x32/camera_black.png"));
    toolButton_image->setToolTip("image");

    toolButton_audio = new QToolButton;
    toolButton_audio->setIcon(QIcon(":/img/32x32/music.png"));
    toolButton_audio->setToolTip("audio");

    hbox->addWidget(filter_label);
    hbox->addWidget(toolButton_video);
    hbox->addWidget(toolButton_image);
    hbox->addWidget(toolButton_audio);
    //hbox->addSpacing(100);
    hbox->addWidget(media_label);
    hbox->addWidget(media_label_counter);
    hbox->addWidget(upload_label);
    hbox->addWidget(upload_label_counter);
    hbox->addWidget(download_label);
    hbox->addWidget(download_label_counter);

    vbox->addWidget(widget);


    switch(sphere_data.scope)
    {
    case Sphere_scope::PRIVATE :
        // check directory exist
        if (sphere_data.directory.isEmpty())
        {
            //uuid = QUuid::createUuid().toString().remove("-").replace(0, 1, "");
            //uuid = uuid.replace(uuid.size()-1, 1, "");
            //sphere_data.directory = sphere_data.title.toLower().trimmed() + "_" + uuid;

            sphere_data.directory = gen_directory(sphere_data.title);
        }

        nodecast_datas = prefs.getSavePath() + "/nodecast/spheres/private/";
        check_dir = nodecast_datas.absolutePath() + "/" + sphere_data.directory;
        if (!check_dir.exists())
        {
            nodecast_datas.mkdir(sphere_data.directory);
            nodecast_datas.mkdir(sphere_data.directory + "/torrents");
        }
        m_color = new QColor(105,105,105);




        media_scroll = new QScrollArea;
        media_scroll->setWidgetResizable(true);
        media_scroll->setFrameStyle(QFrame::NoFrame);
        media_container = new QWidget(media_scroll);
        flowLayout = new FlowLayout(media_container);
        connect(flowLayout, SIGNAL(emit_deleted_torrent()), this, SLOT(deleted_torrent()));

        media_scroll->setWidget(media_container);

        vbox->addWidget(media_scroll);

        index_tab = parent->addWidget(widget_container);
        qDebug() << "INDEX TAB : " << index_tab;

        m_room = new Room(sphere_data, m_stacked_room);

        ScanFoldersModel::instance()->addPath(nodecast_datas.absolutePath() + "/" + sphere_data.directory, true);
        connect(m_scanFolders, SIGNAL(torrentsAdded(QStringList&)), this, SLOT(torrentsAdded(QStringList&)));

        break;

    case Sphere_scope::PUBLIC :
        m_color = new QColor(119,136,153);

        media_scroll = new QScrollArea();
        media_scroll->setWidgetResizable(true);
        media_scroll->setFrameStyle(QFrame::NoFrame);
        media_container = new QWidget(media_scroll);
        flowLayout = new FlowLayout(media_container);

        media_scroll->setWidget(media_container);

        view = new QWebView();
        view->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
        view->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
        view->settings()->setAttribute(QWebSettings::ZoomTextOnly, true);
        view->settings()->setAttribute(QWebSettings::AutoLoadImages, true);
        view->settings()->setAttribute(QWebSettings::NotificationsEnabled, false);
        view->setZoomFactor(0.8);
        view->load(QUrl(sphere_data.url));

        hSplitter = new QSplitter(Qt::Vertical);
        hSplitter->addWidget(view);
        hSplitter->addWidget(media_scroll);

        index_tab = parent->addWidget(hSplitter);
        qDebug() << "INDEX TAB : " << index_tab;

        break;

    case Sphere_scope::FIXED :
        m_color = new QColor(100, 143, 000);
        view = new QWebView();
        view->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
        view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
        view->settings()->setAttribute(QWebSettings::ZoomTextOnly, true);
        view->settings()->setAttribute(QWebSettings::AutoLoadImages, true);
        view->settings()->setAttribute(QWebSettings::NotificationsEnabled, false);
        view->setZoomFactor(0.8);
        view->load(QUrl(sphere_data.url));
        index_tab = parent->addWidget(view);

        //content = new QWidget();
        //index_tab = parent->addWidget(content);
        qDebug() << "INDEX TAB : " << index_tab;

        break;
    }


//    if (scope.compare("private") == 0)
//        m_color = new QColor(105,105,105);
//    else if (scope.compare("public") == 0)
//        m_color = new QColor(119,136,153);
//    else
//        m_color = new QColor(100, 143, 000);


    setAcceptDrops(true);
    setAutoExclusive(true);
    setCheckable(true);

    m_index = index++;


//    initStyleOption(QStyleOptionButton::Flat);
}

bool Sphere::isScopeFixed()
{
    bool scope = (sphere_data.scope == Sphere_scope::FIXED)? true : false;
    return scope;
}


bool Sphere::isScopePrivate()
{
    bool scope = (sphere_data.scope == Sphere_scope::PRIVATE)? true : false;
    return scope;
}


bool Sphere::isScopePublic()
{
    bool scope = (sphere_data.scope == Sphere_scope::PUBLIC)? true : false;
    return scope;
}


int Sphere::getRoomIndex()
{
    if (m_room)
    {
        qDebug() << "ROOM INDEX : " << m_room->index_tab;
        return m_room->index_tab;
    }
    return -1;
}


void Sphere::reloadWeb()
{
    qDebug() << "RELOAD WEB";
    view->load(QUrl(sphere_data.url));
}


QSize Sphere::sizeHint() const
{
    return QSize(105,105);
}



Sphere::~Sphere()
{
    qDebug() << "DELETE SPHERE";
    if (m_room) delete m_room;
}


void Sphere::dragEnterEvent(QDragEnterEvent *event)
{
    if (sphere_data.scope == Sphere_scope::FIXED) return;

    if(event->mimeData()->hasUrls() || event->mimeData()->hasFormat("application/vnd.text.list"))
        event->acceptProposedAction();
}

void Sphere::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat("application/vnd.text.list"))
    {
        QByteArray encodedData = event->mimeData()->data("application/vnd.text.list");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        QStringList jids;

        while (!stream.atEnd()) {
                QString text;
                stream >> text;
                jids << text;
            }
        qDebug() << " Sphere::dropEvent application/vnd.text.list : " << jids;

        m_room->send_invitation(jids);
    }
    else if (event->mimeData()->hasUrls())
    {

        QList<QUrl> droppedUrls = event->mimeData()->urls();
        int droppedUrlCnt = droppedUrls.size();

        for(int i = 0; i < droppedUrlCnt; i++)
        {
           QString localPath = droppedUrls[i].toLocalFile();
           QFileInfo fileInfo(localPath);

           if(fileInfo.isFile())
           {
               QString extension = fsutils::fileExtension(fileInfo.fileName());
               qDebug() << "EXTENSION FILE : " << extension;

               if (extension == "torrent")
               {
                   if (!fsutils::isValidTorrentFile(localPath))
                   {
                       qDebug() << "torrent is not valid : " << fileInfo.fileName();
                       return;
                   }

                   qDebug() << "copy torrent to the sphere directory : " << fileInfo.fileName();
                   QDir nodecast_datas;
                   nodecast_datas = prefs.getSavePath() + "/nodecast/spheres/private/";
                   QFile::copy(fileInfo.absoluteFilePath(), nodecast_datas.absolutePath() + "/" + sphere_data.directory + "/" + fileInfo.fileName() );
                   return;
               }

               QString target_link = prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_data.directory + "/" + fileInfo.fileName();
               fsutils::forceRemove(target_link);
               QFileInfo fileInfoLink(target_link);

               bool check_link = fsutils::createLink(localPath, fileInfoLink.absoluteFilePath());
               QString link_status = check_link? "LINK IS OK" : "LINK IS KO";
               qDebug() << link_status;
               if (!check_link)
               {
                   QMessageBox::information(this, tr("Create link failed in %1 sphere").arg(sphere_data.title), fileInfo.absoluteFilePath());
                   return;
               }

               qDebug() << "FILE SIZE : " << fileInfo.size();

               if (fileInfo.size() < 1000000)
               {
                   // if file size < 1Mo send it through XMPP.
                   qDebug() << "FILE SIZE < 1Mo : " << fileInfo.size();
                   sendXmppFile(fileInfoLink.absoluteFilePath());

                   Widgettorrent *wt = new Widgettorrent(sphere_data);
                   connect(wt, SIGNAL(emit_deleted(QWidget*)), flowLayout, SLOT(delItem(QWidget*)));
                   wt->addFile(target_link);
                   flowLayout->addWidget(wt);
               }
               else
               {
                   createTorrentDlg = new TorrentCreatorDlg(sphere_data.directory, fileInfoLink.fileName(), fileInfoLink.absoluteFilePath(), this);
                   connect(createTorrentDlg, SIGNAL(torrent_to_seed(QString, bool)), this, SLOT(addTorrent(QString, bool)));
               }

               //QMessageBox::information(this, tr("Dropped file in %1 sphere").arg(sphere_data.title), fileInfo.absoluteFilePath());
           }
           else if(fileInfo.isDir())
           {
               if (!localPath.endsWith("/")) localPath.append("/"); // workaround a strange behaviour between Mac and Linux ...
               QFileInfo fileInfo(localPath);

               QDir take_dir = fileInfo.dir();
               QString target_link = prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_data.directory + "/" + take_dir.dirName();
               fsutils::forceRemove(target_link);
               qDebug() << "TARGET LINK : " << target_link;
               QFileInfo fileInfoLink(target_link);

               bool check_link = fsutils::createLinkDir(localPath, fileInfoLink.absoluteFilePath());
               QString link_status = check_link? "LINK IS OK" : "LINK IS KO";
               qDebug() << link_status;
               if (!check_link)
               {
                   QMessageBox::information(this, tr("Create link failed in %1 sphere").arg(sphere_data.title), fileInfo.absoluteFilePath());
                   return;
               }

               createTorrentDlg = new TorrentCreatorDlg(sphere_data.directory, fileInfoLink.fileName(), fileInfoLink.absoluteFilePath(), this);
               connect(createTorrentDlg, SIGNAL(torrent_to_seed(QString, bool)), this, SLOT(addTorrent(QString, bool)));
               //QMessageBox::information(this, tr("Dropped directory in %1 sphere").arg(sphere_data.title), fileInfo.absoluteFilePath());
           }
           else
           {
               // none
               QMessageBox::information(this, tr("Dropped, but unknown in %1 sphere").arg(sphere_data.title), tr("Unknown: %1").arg(fileInfo.canonicalFilePath()));
           }
        }
    }

    event->acceptProposedAction();
}



void Sphere::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
//    QString css = "background-color: transparent; border: none; color: white";
//    setStyleSheet(css);


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);


    QColor color = this->isChecked() ? QColor(192,192,192) : *m_color;


    //painter.setPen( QColor("#222326") );
    //painter.setOpacity(1.0);
    //painter.setPen( Qt::TransparentMode );
    painter.setBrush(color);
    painter.drawEllipse(0, 0, 100, 100);
    //painter.setPen( Qt::SolidLine );

    //painter.drawText(10, 50, m_title);

    //painter.drawText(60, 70, m_title);

    QRectF rect(10, 10, 80, 90);
    painter.drawText(rect, Qt::AlignCenter, sphere_data.title);



//    painter.drawStaticText(10, 50, QStaticText("<center>" + m_title + "</center>"));


  //  setScaledContents(false);
}



void Sphere::mousePressEvent(QMouseEvent *e)
{

    if(e->button() == Qt::LeftButton)
    {
        qDebug() << "MOUSE RIGHT PRESSED : " << sphere_data.directory;

        this->setChecked(true);
        //this->update();
        qDebug() << "selected : " << sphere_data.title << " index : " << m_index;
        emit row(m_index);
    }
    else if(e->button() == Qt::RightButton)
    {
        qDebug() << "MOUSE RIGHT PRESSED : " << sphere_data.directory;

        if (isScopePrivate())
            displayListMenu();
    }
}




void Sphere::populate()
{
    qDebug() << "Sphere::polulate";   
    nb_torrent = Widgettorrent::populate(sphere_data, flowLayout);
    media_label_counter->setText(QString::number(nb_torrent));
}


void Sphere::connectRoom(QXmppMucRoom *room)
{
    qDebug() << "Sphere::connectRoom";
    if (m_room) m_room->setXMPPRoom(room);
}


void Sphere::joinRoom()
{
    qDebug() << "Sphere::joinRoom";
    if (m_room) m_room->joinXMPPRoom();
}




void Sphere::flushRoom()
{
    qDebug() << "Sphere::flushRoom";
    if (m_room) m_room->flushRoom();
}


void Sphere::receive_message(const QString login, const QString message)
{
    qDebug() << "Sphere::receive_message : " << login << " : " << message;
    m_room->receiveMessage(login , message);
}



void Sphere::addTorrent(const QTorrentHandle &h)
{
    // active sphere on new torrent
    this->setChecked(true);

    Widgettorrent *wt = new Widgettorrent(sphere_data);
    connect(wt, SIGNAL(emit_deleted(QWidget*)), flowLayout, SLOT(delItem(QWidget*)));
    wt->addTorrent(h);
    flowLayout->addWidget(wt);
}



void Sphere::addTorrent(QString path, bool fromScanDir)
{
    qDebug() << "SEND TORRENT : " << path;     
    QString path_dest = path;

    if (fromScanDir)
    {
        // archive file to torrents directory
        QFileInfo fi_torrent(path);
        qDebug() << "COPY FILENAME : " << fi_torrent.fileName();

        QDir nodecast_datas;
        nodecast_datas = prefs.getSavePath() + "/nodecast/spheres/private/";
        path_dest = nodecast_datas.absolutePath() + "/" + sphere_data.directory + "/torrents/" + fi_torrent.fileName();
        QFile::copy(fi_torrent.absoluteFilePath(), path_dest);
    }

    qDebug() << "ROOM : " << m_room->get_name();

    /*
    QStringList users_list = m_room->get_users();
    qDebug() << "Sphere::addTorrent USERS LIST : " << users_list;

    foreach(QString user, users_list)
    {
        Xmpp_client::instance()->sendFile(user, path_dest);
    }
    QString filename = path.split("/").takeLast();
    m_room->send_message(" share : " + filename);
    */
    sendXmppFile(path_dest);


    // add torrent to seed file
    QBtSession::instance()->addTorrent(path, fromScanDir);
    nb_torrent++;
    media_label_counter->setText(QString::number(nb_torrent));
}



void Sphere::addFile(const QString &file_path)
{
    qDebug() << "Sphere::addFile : " << file_path;

    // active sphere on new torrent
    this->setChecked(true);

    const QFileInfo fileInfo(file_path);

    Widgettorrent *wt = new Widgettorrent(sphere_data);
    connect(wt, SIGNAL(emit_deleted(QWidget*)), flowLayout, SLOT(delItem(QWidget*)));
    wt->addFile(fileInfo);
    flowLayout->addWidget(wt);
}


void Sphere::sendXmppFile(QString path_file)
{
    // send file through XMPP. Torrent file or direct file if < 1Mo

    QStringList users_list = m_room->get_users();

    qDebug() << "Sphere::sendXmppFile USERS LIST : " << users_list;

    foreach(QString user, users_list)
    {
        Xmpp_client::instance()->sendFile(user, path_file);
    }
    if (!users_list.isEmpty())
    {
        QString filename = path_file.split("/").takeLast();
        m_room->send_message(" share : " + filename);
    }

}


void Sphere::torrentsAdded(QStringList &torrents)
{
    qDebug() << "SPHERE::TORRENTSADDED" << torrents;
    foreach(QString torrent, torrents)
    {
        qDebug() << "torrentsAdded TORRENT : " << torrent;

        if (!torrent.contains(sphere_data.directory))
        {
            qDebug() << "THIS TORRENT IS NOT FOR MINE !! : " << torrent;
            continue;
        }

        if (!fsutils::isValidTorrentFile(torrent))
        {
            qDebug() << "torrent is not valid : " << torrent;
            fsutils::forceRemove(torrent);
            return;
        }
        addTorrent(torrent, true);
    }
}





void Sphere::displayListMenu() {
    // Create actions

    QAction actionOpen_destination_folder(IconProvider::instance()->getIcon("inode-directory"), tr("Open destination folder"), 0);
    connect(&actionOpen_destination_folder, SIGNAL(triggered()), this, SLOT(openSelectedSphereFolder()));

    QAction actionRename(IconProvider::instance()->getIcon("edit-rename"), tr("Rename..."), 0);
    connect(&actionRename, SIGNAL(triggered()), this, SLOT(renameSelectedSphere()));

    QAction actionDelete(IconProvider::instance()->getIcon("edit-delete"), tr("Delete", "Delete the torrent"), 0);
    connect(&actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelectedSphere()));
    // End of actions
    QMenu listMenu(this);
    qDebug("Displaying menu");
    listMenu.addSeparator();
    listMenu.addAction(&actionOpen_destination_folder);
    listMenu.addAction(&actionRename);

    listMenu.addSeparator();
    listMenu.addAction(&actionDelete);
    listMenu.addSeparator();

    // Call menu
    QAction *act = 0;
    act = listMenu.exec(QCursor::pos());
}




void Sphere::deleteSelectedSphere() {


    QString message = "are you sure you want delete <B>%1</B> sphere ?";

    int retButton = QMessageBox::question(
            new QWidget, "Delete sphere", message.arg(sphere_data.title),
            QMessageBox::Yes, QMessageBox::No);


    switch(retButton)
    {
    case QMessageBox::Yes:
        {
            qDebug() << "Sphere::deleteSelectedSphere : "  << sphere_data.title;

            if (nb_torrent > 0)
            {
                QMessageBox::warning(this, tr("Failure"), tr("please delete %1 torrent(s) before.").arg(nb_torrent));
                return;
            }
            else
            {
                bool res = Xmpp_client::instance()->deleteRoom(sphere_data.directory);
                if (res)
                {
                    qDebug() << "SPHERE DELETED ROOM : " << m_room->get_name();
                    m_room->flushRoom();

                    QDir sphere_directory(prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_data.directory);
                    if (sphere_directory.exists())
                    {
                        bool bool_delete = sphere_directory.removeRecursively();
                        QString res_deleted = bool_delete? "TRUE" : "FALSE";
                        qDebug() << "SPHERE DIR DELETED : " << res_deleted;
                    }
                    emit emit_deleted(this);
                }
            }
        }
        break;
    case QMessageBox::No:
        {
            return;
        }
        break;
    default:
        break;
    }

}


void Sphere::openSelectedSphereFolder() const
{
      QString sphereFolder =  prefs.getSavePath() + "/nodecast/spheres/private/" + sphere_data.directory;
      qDebug("Opening path at %s", qPrintable(sphereFolder));
      QDesktopServices::openUrl(QUrl::fromLocalFile(sphereFolder));
}


void Sphere::renameSelectedSphere()
{
    qDebug() << "Sphere::renameSelectedSphere : " << sphere_data.title;
}


void Sphere::deleted_torrent()
{
    if (nb_torrent > 0) nb_torrent--;
    media_label_counter->setText(QString::number(nb_torrent));
}

QGroupBox * Sphere::getRoomWidget()
{
    if (m_room) return m_room->getWidget();
}

