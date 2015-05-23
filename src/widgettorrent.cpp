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

#include "widgettorrent.h"
#include "ui_widgettorrent.h"

Widgettorrent *last_widget = NULL;

void Widgettorrent::unckeck_widget_selected(Widgettorrent *wt)
{
    if (last_widget)
        last_widget->ui->label_thumbnail->setFrameShape(QFrame::NoFrame);
    last_widget = wt;
    last_widget->ui->label_thumbnail->setFrameShape(QFrame::StyledPanel);
}


int Widgettorrent::populate(Sphere_data a_sphere_data, FlowLayout *parent)
{
    Preferences prefs;

    qDebug() << "POPULATE : " << prefs.getSavePath() + "/nodecast/spheres/private/" + a_sphere_data.directory;

    QDir sphere_dir(prefs.getSavePath() + "/nodecast/spheres/private/" + a_sphere_data.directory);
    QFileInfoList file_list = sphere_dir.entryInfoList(QDir::NoDotAndDotDot |QDir::Files);
    //qDebug() << "Widgettorrent::populate FILE LIST : " << file_list.first().fileName();

    std::vector<libtorrent::torrent_handle> torrents = QBtSession::instance()->getSession()->get_torrents();

    std::vector<libtorrent::torrent_handle>::const_iterator it = torrents.begin();
    std::vector<libtorrent::torrent_handle>::const_iterator itend = torrents.end();
    int counter=0;
    for ( ; it != itend; ++it)
    {
        const QTorrentHandle h(*it);
        //    if (HiddenData::hasData(h.hash()))
        //      continue;

        // get path
        qDebug() << "SAVE PATH : "<< h.save_path();

        QFileInfo torrent_info(h.save_path_parsed());
        qDebug() << "TORRENT INFO : " << torrent_info.fileName();

        QString path = h.save_path();
        // remove last / of the path, split on / and take the directory name
        QString dir = path.remove(path.size()-1, 1).split("/").takeLast();

        if (dir != a_sphere_data.directory) continue;

        counter++;

        qDebug() << "PATH : " << path << " TITLE " << a_sphere_data.title;
        Widgettorrent *wt = new Widgettorrent(a_sphere_data);
        QObject::connect(wt, SIGNAL(emit_deleted(QWidget*)), parent, SLOT(delItem(QWidget*)));

        parent->addWidget(wt);
        qDebug() << "Widgettorrent::populate";
        wt->addTorrent(h);

        int index = file_list.indexOf(torrent_info);
        if(index != -1)
            file_list.removeAt(index);


       // QObject::connect(QBtSession::instance(), SIGNAL(addedTorrent(QTorrentHandle)), wt, SLOT(addTorrent(QTorrentHandle)));

    }

    foreach (QFileInfo file, file_list)
    {
        qDebug() << "FILE NOT TORRENT : " << file.fileName();
        counter++;

        Widgettorrent *wt = new Widgettorrent(a_sphere_data);
        QObject::connect(wt, SIGNAL(emit_deleted(QWidget*)), parent, SLOT(delItem(QWidget*)));

        parent->addWidget(wt);
        qDebug() << "Widgettorrent::populate addFIle : " << file.fileName();
        wt->addFile(file);
    }




    return counter;
}


Widgettorrent::Widgettorrent(Sphere_data a_sphere_data) : sphere_data(a_sphere_data),
    ui(new Ui::widgettorrent)
{
    ui->setupUi(this);
//    ui->label_title->setText(m_title);

    setMinimumSize(200, 130);
    setMaximumSize(200, 130);
    videoPlayer = NULL;
    timer_get_torrent_progress = NULL;
}

Widgettorrent::~Widgettorrent()
{
    qDebug() << "DELETE WIDGET TORRENT";
    last_widget = NULL;

    if (videoPlayer != 0)
    {
        //qDebug() << "process ID = " << videoPlayer->processId();
        videoPlayer->kill();
        delete(videoPlayer);
    }

    if (timer_get_torrent_progress)
        timer_get_torrent_progress->stop();

    delete ui;
    qDebug() << "DELETE WIDGET TORRENT UI";
}


void Widgettorrent::mousePressEvent(QMouseEvent *e)
{

    if(e->button() == Qt::LeftButton)
    {
        qDebug() << "MOUSE PRESSED : " << ui->label_title->text();

        unckeck_widget_selected(this);
    }
    else if(e->button() == Qt::RightButton)
    {
        qDebug() << "MOUSE RIGHT PRESSED : " << ui->label_title->text();

        if (torrent_data.is_torrent)
            displayListMenuTorrent();
        else
            displayListMenuFile();
    }

}



void Widgettorrent::mouseDoubleClickEvent(QMouseEvent *e)
{
    mFirstX=0;
    mFirstY=0;
//    mFirstClick=true;
//    mpaintflag=false;

    if(e->button() == Qt::LeftButton)
            {
                //store 1st point
              //  if(mFirstClick)
              //  {
                    mFirstX = e->x();
                    mFirstY = e->y();
                  //  mFirstClick = false;
                  //  mpaintflag = true;
                    qDebug() << "First image's coordinates" << mFirstX << "," << mFirstY << " title " << torrent_data.file;
                    emit emit_title();
                  //  update();

                //}

            }
}

void Widgettorrent::on_media_doubleClicked()
{

    // open with VLC if media type is a video or a directory
    if (torrent_data.type == "video")
    {

        qDebug() << "LAUNCH PLAYER : ";
/*
        QString program;
        QStringList arguments;

    #if defined (Q_OS_MAC)
        program = "open";
        arguments << "-a" << Preferences().getVideoPlayer() << m_torrent.absolute_files_path();
    #else
        program = Preferences().getVideoPlayer();
        arguments << m_torrent.absolute_files_path();
    #endif

        qDebug() << "program : " << program;

        videoPlayer = new QProcess(this);
        videoPlayer->startDetached(program, arguments);
  */

        QString dir = Preferences().getSavePath() + "/nodecast/spheres/private/" + sphere_data.directory + "/" + torrent_data.file;
        QString path = QDir::toNativeSeparators(dir);
        qDebug() << "OPEN VIDEO TO : " << path;
        QDesktopServices::openUrl(QUrl("file:///" + path));


    }
    else if (torrent_data.type == "binary")
    {
        QString dir = Preferences().getSavePath() + "/nodecast/spheres/private/" + sphere_data.directory;
        QString path = QDir::toNativeSeparators(dir);
        qDebug() << "LAUNCH FILE EXPLORER TO : " << path;
        QDesktopServices::openUrl(QUrl("file:///" + path));
    }
    else if (torrent_data.type == "image" || torrent_data.type == "pdf" || torrent_data.type == "text")
    {
        QString dir = Preferences().getSavePath() + "/nodecast/spheres/private/" + sphere_data.directory + "/" + torrent_data.file;
        QString path = QDir::toNativeSeparators(dir);
        qDebug() << "OPEN PICTURE/PDF/TEXT TO : " << path;
        QDesktopServices::openUrl(QUrl("file:///" + path));
    }
    else if (torrent_data.type == "directory")
    {
        QString dir = Preferences().getSavePath() + "/nodecast/spheres/private/" + sphere_data.directory + "/" + torrent_data.file;
        QString path = QDir::toNativeSeparators(dir);
        qDebug() << "OPEN DIRECTORY : " << path;
        QDesktopServices::openUrl(QUrl("file:///" + path));
    }
}


//void Widgettorrent::populate()
//{
    // Load the torrents


//    std::vector<libtorrent::torrent_handle> torrents = QBtSession::instance()->getSession()->get_torrents();

//  std::vector<libtorrent::torrent_handle>::const_iterator it = torrents.begin();
//  std::vector<libtorrent::torrent_handle>::const_iterator itend = torrents.end();
//  for ( ; it != itend; ++it) {
//    const QTorrentHandle h(*it);
//    if (HiddenData::hasData(h.hash()))
//      continue;
//    addTorrent(h);
//  }
  // Refresh timer
//  connect(&m_refreshTimer, SIGNAL(timeout()), SLOT(forceModelRefresh()));
//  m_refreshTimer.start(m_refreshInterval);
  // Listen for torrent changes
//  connect(QBtSession::instance(), SIGNAL(addedTorrent(QTorrentHandle)), SLOT(addTorrent(QTorrentHandle)));
//  connect(QBtSession::instance(), SIGNAL(torrentAboutToBeRemoved(QTorrentHandle)), SLOT(handleTorrentAboutToBeRemoved(QTorrentHandle)));
//  connect(QBtSession::instance(), SIGNAL(deletedTorrent(QString)), SLOT(removeTorrent(QString)));
//  connect(QBtSession::instance(), SIGNAL(finishedTorrent(QTorrentHandle)), SLOT(handleFinishedTorrent(QTorrentHandle)));
//  connect(QBtSession::instance(), SIGNAL(metadataReceived(QTorrentHandle)), SLOT(handleTorrentUpdate(QTorrentHandle)));
//  connect(QBtSession::instance(), SIGNAL(resumedTorrent(QTorrentHandle)), SLOT(handleTorrentUpdate(QTorrentHandle)));
//  connect(QBtSession::instance(), SIGNAL(pausedTorrent(QTorrentHandle)), SLOT(handleTorrentUpdate(QTorrentHandle)));
//  connect(QBtSession::instance(), SIGNAL(torrentFinishedChecking(QTorrentHandle)), SLOT(handleTorrentUpdate(QTorrentHandle)));
//}


void Widgettorrent::job_progress(qint64 done, qint64 total)
{
    qDebug() << "Widgettorrent::job_progress Transmission progress:" << done << "/" << total;

    //float tmp = done * 100;
    //qDebug() << "PROGRESS float * 100 : " << m_torrent.name() << " : " << tmp;

    //int progress = int(tmp + 0.5);
    int progress = done * 100 / total;
    ui->progressBar_torrent->setValue(progress);
}

void Widgettorrent::addFile(const QFileInfo &file, QXmppTransferJob *job)
{
    qDebug() << "Widgettorrent::addFile";
    connect(this, SIGNAL(emit_title()), this, SLOT(on_media_doubleClicked()));
    if (job)
    {
        bool check = connect(job, SIGNAL(progress(qint64,qint64)), this, SLOT(job_progress(qint64,qint64)));
        Q_ASSERT(check);
        check = connect(job, SIGNAL(finished()), this, SLOT(update_torrent_type_thumbnail()));
        Q_ASSERT(check);
    }

    m_title.setText(file.fileName());
    ui->label_title->setText(file.fileName());
    torrent_data.file = file.fileName();
    torrent_data.filepath = file.absoluteFilePath();
    torrent_data.dirpath = file.absolutePath();
    torrent_data.is_torrent = false;



    qDebug() << "Widgettorrent::addFile save_path_parsed : " << file.absoluteFilePath();

    QString extension = fsutils::fileExtension(file.fileName());
    qDebug() << "FILE EXTENSION : " << extension;

    if (!extension.isEmpty())
    {
        torrent_data.extension=extension;

        QString extension_thumbnail_path = ":/img/32x32/file_extension/file_extension_" + extension + ".png";
        QPixmap pic = QPixmap(extension_thumbnail_path);
        ui->label_thumbnail->setPixmap(pic);
    }
    torrent_data.type = "binary";

    update_torrent_type_thumbnail();
}


void Widgettorrent::addTorrent(const QTorrentHandle &h)
{
    qDebug() << "Widgettorrent::addedTorrent";
    connect(this, SIGNAL(emit_title()), this, SLOT(on_media_doubleClicked()));

    m_torrent = h;
    m_title.setText(h.name());
    ui->label_title->setText(h.name());
    torrent_data.file = h.name();
    torrent_data.hash = h.hash();



    qDebug() << "Widgettorrent::addTorrent save_path_parsed : " << h.save_path_parsed();

    QFileInfo fileInfo(h.save_path_parsed());
    if(fileInfo.isFile())
    {
        // torrent is a file but unknown type at this moment
        torrent_data.type = "binary";
    }
    else if(fileInfo.isDir())
    {
        // torrent is a directory
        torrent_data.type = "directory";
    }


    qDebug() << "DATA TYPE : " << torrent_data.type;


    timer_get_torrent_progress = new QTimer(this);
    connect(timer_get_torrent_progress, SIGNAL(timeout()), this, SLOT(update_timer_torrent_progress()));
    timer_get_torrent_progress->start(1000);


//  if (torrentRow(h.hash()) < 0) {
//    beginInsertTorrent(m_torrents.size());
//    TorrentModelItem *item = new TorrentModelItem(h);
//    connect(item, SIGNAL(labelChanged(QString,QString)), SLOT(handleTorrentLabelChange(QString,QString)));
//    m_torrents << item;
//    emit torrentAdded(item);
//    endInsertTorrent();
//  }
}



void Widgettorrent::update_timer_torrent_progress()
{

    // check name because of torrent metadata lag
    if (ui->label_title->text().isEmpty()  && !m_torrent.name().isEmpty())
        ui->label_title->setText(m_torrent.name());


    // set the torrent's thumbnail according extension file
    if (torrent_data.type != "directory" && torrent_data.extension.isEmpty()  && !m_torrent.name().isEmpty())
    {
        QString extension = fsutils::fileExtension(m_torrent.name());
        qDebug() << "TORRENT EXTENSION : " << extension;

        if (!extension.isEmpty())
        {
            torrent_data.extension=extension;

            QString extension_thumbnail_path = ":/img/32x32/file_extension/file_extension_" + extension + ".png";
            QPixmap pic = QPixmap(extension_thumbnail_path);
            ui->label_thumbnail->setPixmap(pic);
        }
    }



 //   qDebug() << "PROGRESS : " << m_torrent.progress();;

    float tmp = m_torrent.progress() * 100;
    //qDebug() << "PROGRESS float * 100 : " << m_torrent.name() << " : " << tmp;

    int progress = int(tmp + 0.5);
    ui->progressBar_torrent->setValue(progress);

    if (tmp >= 100)
    {
        this->timer_get_torrent_progress->stop();
        update_torrent_type_thumbnail();
    }
}


// when torrent is completly download, I can check his mime type. do not use extension file
void Widgettorrent::update_torrent_type_thumbnail()
{
    qDebug() << "Widgettorrent::update_torrent_type_thumbnail";

    if (torrent_data.type != "directory")
    {


        updateThumbnail *thumbnailtask = new updateThumbnail(ui, &torrent_data, &m_torrent);
        thumbnailtask->setAutoDelete(true);
        QThreadPool::globalInstance()->start(thumbnailtask);


        /*
        QMutexLocker locker(&global_mutex::thumbnail_mutex);
        qDebug() << "Widgettorrent::update_torrent_type_thumbnail UNLOCKED MUTEX";


        QString save_path;
        if (torrent_data.is_torrent)
            save_path = m_torrent.save_path_parsed();
        else
            save_path = torrent_data.filepath;

        qDebug() << "Widgettorrent::update_torrent_type_thumbnail : " << save_path;

        QMimeType mime = m_mime_db.mimeTypeForFile(save_path);

        if (mime.inherits("image/gif") ||
                 mime.inherits("image/jpeg") ||
                 mime.inherits("image/png") ||
                 mime.inherits("image/tiff"))
        {
            QByteArray imageFormat = QImageReader::imageFormat(save_path); //Where fileName - path to your file
            qDebug() << "imageFormat : " << imageFormat;

            QPixmap img;

            if (imageFormat.size() != 0 && img.load(save_path))
            {
                QPixmap thumbnail = img.scaled(100, 50, Qt::IgnoreAspectRatio, Qt::FastTransformation);
                ui->label_thumbnail->setPixmap(thumbnail);
            }
            torrent_data.type = "image";
        }
        else if (mime.inherits("text/css") ||
                 mime.inherits("text/csv") ||
                 mime.inherits("text/html") ||
                 mime.inherits("text/plain") ||
                 mime.inherits("text/xml"))
        {
            torrent_data.type = "text";
        }
        else if (mime.inherits("video/mpeg") ||
                 mime.inherits("video/mp4") ||
                 mime.inherits("video/quicktime") ||
                 mime.inherits("video/x-ms-wmv") ||
                 mime.inherits("video/x-msvideo") ||
                 mime.inherits("video/x-flv") ||
                 mime.inherits("video/webm") ||
                 mime.inherits("video/x-matroska")
                 )
        {
            torrent_data.type = "video";
        }
        else if (mime.inherits("audio/mpeg") ||
                 mime.inherits("audio/mp3") ||
                 mime.inherits("audio/x-ms-wma") ||
                 mime.inherits("audio/vnd.rn-realaudio") ||
                 mime.inherits("audio/x-wav") ||
                 mime.inherits("audio/wav") ||
                 mime.inherits("audio/x-matroska")
                 )
        {
            torrent_data.type = "audio";
        }
        else if (mime.inherits("application/pdf"))
        {
            torrent_data.type = "pdf";
        }
        else
        {
            QFileInfo fileInfo(save_path);
            if(fileInfo.isFile())
            {
                // torrent is a file but unknown type, maybe a binary file
                torrent_data.type = "binary";
            }
            else if(fileInfo.isDir())
            {
                // torrent is a directory
                torrent_data.type = "directory";
            }
        }

        */
    }

    //qDebug() << "Widgettorrent::update_torrent_type_thumbnail DATA TYPE : " << torrent_data.type;
}




updateThumbnail::updateThumbnail(Ui::widgettorrent *ui, Torrent_data *torrent_data, QTorrentHandle *torrent_handle) : m_ui(ui), m_torrent_data(torrent_data), m_torrent_handle(torrent_handle)
{
    qDebug() << "updateThumbnail::updateThumbnail CONSTRUCT";
}


void updateThumbnail::run()
{
    qDebug() << "updateThumbnail::run";

        QMutexLocker locker(&global_mutex::thumbnail_mutex);
        qDebug() << "updateThumbnail::update_torrent_type_thumbnail UNLOCKED MUTEX";


        QString save_path;
        if (m_torrent_data->is_torrent)
            save_path = m_torrent_handle->save_path_parsed();
        else
            save_path = m_torrent_data->filepath;

        qDebug() << "Widgettorrent::update_torrent_type_thumbnail : " << save_path;

        QMimeType mime = m_mime_db.mimeTypeForFile(save_path);

        if (mime.inherits("image/gif") ||
                 mime.inherits("image/jpeg") ||
                 mime.inherits("image/png") ||
                 mime.inherits("image/tiff"))
        {
            QByteArray imageFormat = QImageReader::imageFormat(save_path); //Where fileName - path to your file
            qDebug() << "imageFormat : " << imageFormat;

            QPixmap img;

            if (imageFormat.size() != 0 && img.load(save_path))
            {
                QPixmap thumbnail = img.scaled(100, 50, Qt::IgnoreAspectRatio, Qt::FastTransformation);
                m_ui->label_thumbnail->setPixmap(thumbnail);
            }
            m_torrent_data->type = "image";
        }
        else if (mime.inherits("text/css") ||
                 mime.inherits("text/csv") ||
                 mime.inherits("text/html") ||
                 mime.inherits("text/plain") ||
                 mime.inherits("text/xml"))
        {
            m_torrent_data->type = "text";
        }
        else if (mime.inherits("video/mpeg") ||
                 mime.inherits("video/mp4") ||
                 mime.inherits("video/quicktime") ||
                 mime.inherits("video/x-ms-wmv") ||
                 mime.inherits("video/x-msvideo") ||
                 mime.inherits("video/x-flv") ||
                 mime.inherits("video/webm") ||
                 mime.inherits("video/x-matroska")
                 )
        {
            m_torrent_data->type = "video";
        }
        else if (mime.inherits("audio/mpeg") ||
                 mime.inherits("audio/mp3") ||
                 mime.inherits("audio/x-ms-wma") ||
                 mime.inherits("audio/vnd.rn-realaudio") ||
                 mime.inherits("audio/x-wav") ||
                 mime.inherits("audio/wav") ||
                 mime.inherits("audio/x-matroska")
                 )
        {
            m_torrent_data->type = "audio";
        }
        else if (mime.inherits("application/pdf"))
        {
            m_torrent_data->type = "pdf";
        }
        else
        {
            QFileInfo fileInfo(save_path);
            if(fileInfo.isFile())
            {
                // torrent is a file but unknown type, maybe a binary file
                m_torrent_data->type = "binary";
            }
            else if(fileInfo.isDir())
            {
                // torrent is a directory
                m_torrent_data->type = "directory";
            }
        }

    qDebug() << "updateThumbnail::run DATA TYPE : " << m_torrent_data->type;
}


void Widgettorrent::displayListMenuTorrent() {
  // Create actions
  QAction actionStart(IconProvider::instance()->getIcon("media-playback-start"), tr("Resume", "Resume/start the torrent"), 0);
  connect(&actionStart, SIGNAL(triggered()), this, SLOT(startSelectedTorrents()));
  QAction actionPause(IconProvider::instance()->getIcon("media-playback-pause"), tr("Pause", "Pause the torrent"), 0);
  connect(&actionPause, SIGNAL(triggered()), this, SLOT(pauseSelectedTorrents()));
  QAction actionDelete(IconProvider::instance()->getIcon("edit-delete"), tr("Delete", "Delete the torrent"), 0);
  connect(&actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelectedTorrents()));
  QAction actionPreview_file(IconProvider::instance()->getIcon("view-preview"), tr("Preview file..."), 0);
  QAction actionSet_max_ratio(QIcon(QString::fromUtf8(":/Icons/skin/ratio.png")), tr("Limit share ratio..."), 0);
  connect(&actionSet_max_ratio, SIGNAL(triggered()), this, SLOT(setMaxRatioSelectedTorrents()));
  QAction actionSet_upload_limit(QIcon(QString::fromUtf8(":/Icons/skin/seeding.png")), tr("Limit upload rate..."), 0);
  connect(&actionSet_upload_limit, SIGNAL(triggered()), this, SLOT(setUpLimitSelectedTorrents()));
  QAction actionSet_download_limit(QIcon(QString::fromUtf8(":/Icons/skin/download.png")), tr("Limit download rate..."), 0);
  connect(&actionSet_download_limit, SIGNAL(triggered()), this, SLOT(setDlLimitSelectedTorrents()));
  QAction actionOpen_destination_folder(IconProvider::instance()->getIcon("inode-directory"), tr("Open destination folder"), 0);
  connect(&actionOpen_destination_folder, SIGNAL(triggered()), this, SLOT(openSelectedTorrentsFolder()));
  QAction actionIncreasePriority(IconProvider::instance()->getIcon("go-up"), tr("Move up", "i.e. move up in the queue"), 0);
  connect(&actionIncreasePriority, SIGNAL(triggered()), this, SLOT(increasePrioSelectedTorrents()));
  QAction actionDecreasePriority(IconProvider::instance()->getIcon("go-down"), tr("Move down", "i.e. Move down in the queue"), 0);
  connect(&actionDecreasePriority, SIGNAL(triggered()), this, SLOT(decreasePrioSelectedTorrents()));
  QAction actionTopPriority(IconProvider::instance()->getIcon("go-top"), tr("Move to top", "i.e. Move to top of the queue"), 0);
  connect(&actionTopPriority, SIGNAL(triggered()), this, SLOT(topPrioSelectedTorrents()));
  QAction actionBottomPriority(IconProvider::instance()->getIcon("go-bottom"), tr("Move to bottom", "i.e. Move to bottom of the queue"), 0);
  connect(&actionBottomPriority, SIGNAL(triggered()), this, SLOT(bottomPrioSelectedTorrents()));
  QAction actionSetTorrentPath(IconProvider::instance()->getIcon("inode-directory"), tr("Set location..."), 0);
  QAction actionForce_recheck(IconProvider::instance()->getIcon("document-edit-verify"), tr("Force recheck"), 0);
  connect(&actionForce_recheck, SIGNAL(triggered()), this, SLOT(recheckSelectedTorrents()));
  QAction actionCopy_magnet_link(QIcon(":/Icons/magnet.png"), tr("Copy magnet link"), 0);
  QAction actionSuper_seeding_mode(tr("Super seeding mode"), 0);
  actionSuper_seeding_mode.setCheckable(true);
  connect(&actionSuper_seeding_mode, SIGNAL(triggered()), this, SLOT(toggleSelectedTorrentsSuperSeeding()));
  QAction actionRename(IconProvider::instance()->getIcon("edit-rename"), tr("Rename..."), 0);
  connect(&actionRename, SIGNAL(triggered()), this, SLOT(renameSelectedTorrent()));
  QAction actionSequential_download(tr("Download in sequential order"), 0);
  actionSequential_download.setCheckable(true);
  connect(&actionSequential_download, SIGNAL(triggered()), this, SLOT(toggleSelectedTorrentsSequentialDownload()));
  QAction actionFirstLastPiece_prio(tr("Download first and last piece first"), 0);
  actionFirstLastPiece_prio.setCheckable(true);
  connect(&actionFirstLastPiece_prio, SIGNAL(triggered()), this, SLOT(toggleSelectedFirstLastPiecePrio()));
  // End of actions
  QMenu listMenu(this);
  // Enable/disable pause/start action given the DL state
  bool has_pause = false, has_start = false, has_preview = false;
  bool all_same_super_seeding = true;
  bool super_seeding_mode = false;
  bool all_same_sequential_download_mode = true, all_same_prio_firstlast = true;
  bool sequential_download_mode = false, prioritize_first_last = false;
  bool one_has_metadata = false, one_not_seed = false;
  bool first = true;
  qDebug("Displaying menu");
    // Get the file name
    QString hash = torrent_data.hash;
    qDebug() << "HASH : " << hash;
    // Get handle and pause the torrent
   // h = BTSession->getTorrentHandle(hash);
    QTorrentHandle h =  QBtSession::instance()->getTorrentHandle(hash);
    if (!h.is_valid()) return;
    if (h.has_metadata())
      one_has_metadata = true;
    if (!h.is_seed()) {
      one_not_seed = true;
      if (h.has_metadata()) {
        if (first) {
          sequential_download_mode = h.is_sequential_download();
          prioritize_first_last = h.first_last_piece_first();
        } else {
          if (sequential_download_mode != h.is_sequential_download()) {
            all_same_sequential_download_mode = false;
          }
          if (prioritize_first_last != h.first_last_piece_first()) {
            all_same_prio_firstlast = false;
          }
        }
      }
    }
    else {
      if (!one_not_seed && all_same_super_seeding && h.has_metadata()) {
        if (first) {
          super_seeding_mode = h.super_seeding();
        } else {
          if (super_seeding_mode != h.super_seeding()) {
            all_same_super_seeding = false;
          }
        }
      }
    }
    if (h.is_paused()) {
      if (!has_start) {
        listMenu.addAction(&actionStart);
        has_start = true;
      }
    }else{
      if (!has_pause) {
        listMenu.addAction(&actionPause);
        has_pause = true;
      }
    }
    if (h.has_metadata() && QBtSession::instance()->isFilePreviewPossible(hash) && !has_preview) {
      has_preview = true;
    }
    first = false;
  //  if (has_pause && has_start && has_preview && one_not_seed) break;



  listMenu.addSeparator();
  listMenu.addAction(&actionDelete);
  listMenu.addSeparator();
  //listMenu.addAction(&actionSetTorrentPath);
  //if (selectedIndexes.size() == 1)
  //  listMenu.addAction(&actionRename);
  // Label Menu
/*  QStringList customLabels = getCustomLabels();
  customLabels.sort();
  QList<QAction*> labelActions;
  QMenu *labelMenu = listMenu.addMenu(IconProvider::instance()->getIcon("view-categories"), tr("Label"));
  labelActions << labelMenu->addAction(IconProvider::instance()->getIcon("list-add"), tr("New...", "New label..."));
  labelActions << labelMenu->addAction(IconProvider::instance()->getIcon("edit-clear"), tr("Reset", "Reset label"));
  labelMenu->addSeparator();
  foreach (const QString &label, customLabels) {
    labelActions << labelMenu->addAction(IconProvider::instance()->getIcon("inode-directory"), label);
  }
  */
  /*listMenu.addSeparator();
  if (one_not_seed)
    listMenu.addAction(&actionSet_download_limit);
  listMenu.addAction(&actionSet_max_ratio);
  listMenu.addAction(&actionSet_upload_limit);
  if (!one_not_seed && all_same_super_seeding && one_has_metadata) {
    actionSuper_seeding_mode.setChecked(super_seeding_mode);
    listMenu.addAction(&actionSuper_seeding_mode);
  }
  listMenu.addSeparator();
  bool added_preview_action = false;
  if (has_preview) {
    listMenu.addAction(&actionPreview_file);
    added_preview_action = true;
  }
  if (one_not_seed && one_has_metadata) {
    if (all_same_sequential_download_mode) {
      actionSequential_download.setChecked(sequential_download_mode);
      listMenu.addAction(&actionSequential_download);
      added_preview_action = true;
    }
    if (all_same_prio_firstlast) {
      actionFirstLastPiece_prio.setChecked(prioritize_first_last);
      listMenu.addAction(&actionFirstLastPiece_prio);
      added_preview_action = true;
    }
  }
  if (added_preview_action)
    listMenu.addSeparator();*/
  if (one_has_metadata) {
    listMenu.addAction(&actionForce_recheck);
    listMenu.addSeparator();
  }
  listMenu.addAction(&actionOpen_destination_folder);
  if (QBtSession::instance()->isQueueingEnabled() && one_not_seed) {
    listMenu.addSeparator();
    QMenu *prioMenu = listMenu.addMenu(tr("Priority"));
    prioMenu->addAction(&actionTopPriority);
    prioMenu->addAction(&actionIncreasePriority);
    prioMenu->addAction(&actionDecreasePriority);
    prioMenu->addAction(&actionBottomPriority);
  }
  //listMenu.addSeparator();
  //listMenu.addAction(&actionCopy_magnet_link);
  // Call menu
  QAction *act = 0;
  act = listMenu.exec(QCursor::pos());
}




void Widgettorrent::displayListMenuFile() {
  // Create actions
  QAction actionDelete(IconProvider::instance()->getIcon("edit-delete"), tr("Delete", "Delete file"), 0);
  connect(&actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelectedTorrents()));
  QAction actionOpen_destination_folder(IconProvider::instance()->getIcon("inode-directory"), tr("Open destination folder"), 0);
  connect(&actionOpen_destination_folder, SIGNAL(triggered()), this, SLOT(openSelectedTorrentsFolder()));
  // End of actions
  QMenu listMenu(this);
  // Enable/disable pause/start action given the DL state
  qDebug("Displaying menu");



  listMenu.addSeparator();
  listMenu.addAction(&actionDelete);
  listMenu.addSeparator();

  listMenu.addAction(&actionOpen_destination_folder);
  //listMenu.addSeparator();
  // Call menu
  QAction *act = 0;
  act = listMenu.exec(QCursor::pos());
}


void Widgettorrent::startSelectedTorrents() {
    QBtSession::instance()->resumeTorrent(torrent_data.hash);
}


void Widgettorrent::pauseSelectedTorrents() {
    QBtSession::instance()->pauseTorrent(torrent_data.hash);
}


void Widgettorrent::deleteSelectedTorrents() {                    
    if (torrent_data.is_torrent)
    {
        QTorrentHandle torrent = QBtSession::instance()->getTorrentHandle(torrent_data.hash);
        bool delete_local_files = false;
        if (Preferences().confirmTorrentDeletion() &&
                !DeletionConfirmationDlg::askForDeletionConfirmation(delete_local_files, 1, torrent_data.file))
            return;
        QBtSession::instance()->deleteTorrent(torrent_data.hash, delete_local_files);
        qDebug() << "Widgettorrent::deleteSelectedTorrents";
        emit emit_deleted(this);
    }
    else
    {
        bool delete_local_files = true;
        if (Preferences().confirmTorrentDeletion() &&
                !DeletionConfirmationDlg::askForDeletionConfirmation(delete_local_files, 1, torrent_data.file))
            return;
        qDebug() << "DELETE FILE : " << torrent_data.filepath;
        fsutils::forceRemove(torrent_data.filepath);
        emit emit_deleted(this);
    }
}


void Widgettorrent::openSelectedTorrentsFolder() const {

    if (torrent_data.is_torrent)
    {
        const QTorrentHandle h = QBtSession::instance()->getTorrentHandle(torrent_data.hash);
        if (h.is_valid()) {
          QString rootFolder = h.root_path();
          qDebug("Opening path at %s", qPrintable(rootFolder));
          //if (!pathsList.contains(rootFolder)) {
           // pathsList.insert(rootFolder);
            QDesktopServices::openUrl(QUrl::fromLocalFile(rootFolder));
          //}
        }
    }
    else
    {
        qDebug("Opening path at %s", qPrintable(torrent_data.dirpath));
        //if (!pathsList.contains(rootFolder)) {
         // pathsList.insert(rootFolder);
          QDesktopServices::openUrl(QUrl::fromLocalFile(torrent_data.dirpath));
    }
}


void Widgettorrent::recheckSelectedTorrents() {
  QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Recheck confirmation"), tr("Are you sure you want to recheck the selected torrent(s)?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
  if (ret != QMessageBox::Yes)
    return;
    QBtSession::instance()->recheckTorrent(torrent_data.hash);
}
