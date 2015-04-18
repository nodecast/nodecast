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


void Widgettorrent::populate( QString title, QLayout *parent)
{

    std::vector<libtorrent::torrent_handle> torrents = QBtSession::instance()->getSession()->get_torrents();

    std::vector<libtorrent::torrent_handle>::const_iterator it = torrents.begin();
    std::vector<libtorrent::torrent_handle>::const_iterator itend = torrents.end();
    for ( ; it != itend; ++it)
    {
        const QTorrentHandle h(*it);
        //    if (HiddenData::hasData(h.hash()))
        //      continue;

        // get path
        qDebug() << "SAVE PATH : "<< h.save_path();

        QString path = h.save_path();

        if (!path.contains(title, Qt::CaseSensitive)) continue;


            qDebug() << "PATH : " << path << " TITLE " << title;
        Widgettorrent *wt = new Widgettorrent();
        parent->addWidget(wt);
        qDebug() << "Widgettorrent::populate";
        wt->addTorrent(h);
       // QObject::connect(QBtSession::instance(), SIGNAL(addedTorrent(QTorrentHandle)), wt, SLOT(addTorrent(QTorrentHandle)));

    }

}


Widgettorrent::Widgettorrent() :
    ui(new Ui::widgettorrent)
{
    ui->setupUi(this);
//    ui->label_title->setText(m_title);

    setMinimumSize(200, 130);
    setMaximumSize(200, 130);

    videoPlayer = NULL;
}

Widgettorrent::~Widgettorrent()
{
    if (videoPlayer && videoPlayer->processId() != 0)
    {
        qDebug() << "process ID = " << videoPlayer->processId();
        videoPlayer->kill();
        delete(videoPlayer);
    }

    timer_get_torrent_progress->stop();

    delete ui;
}


void Widgettorrent::mousePressEvent(QMouseEvent *e)
{

    if(e->button() == Qt::LeftButton)
    {
        qDebug() << "MOUSE PRESSED : " << ui->label_title->text();

        unckeck_widget_selected(this);
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
                    qDebug() << "First image's coordinates" << mFirstX << "," << mFirstY << " title " << m_torrent.name();
                    emit emit_title();
                  //  update();

                //}

            }
}

void Widgettorrent::on_media_doubleClicked()
{
    qDebug() << "LAUNCH PLAYER : ";

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
    videoPlayer->start(program, arguments);
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


void Widgettorrent::addTorrent(const QTorrentHandle &h)
{
    qDebug() << "Widgettorrent::addedTorrent";
    connect(this, SIGNAL(emit_title()), this, SLOT(on_media_doubleClicked()));

    m_torrent = h;
    m_title.setText(h.name());
    ui->label_title->setText(h.name());


    timer_get_torrent_progress = new QTimer(this);
    connect(timer_get_torrent_progress, SIGNAL(timeout()), this, SLOT(update_timer_torrent_progress()));
    timer_get_torrent_progress->start(1000);


    qDebug() << " save_path_parsed : " << h.save_path_parsed();
    QByteArray imageFormat = QImageReader::imageFormat(h.save_path_parsed()); //Where fileName - path to your file
    qDebug() << "imageFormat : " << imageFormat;

    QPixmap img;

    if (imageFormat.size() != 0 && img.load(h.save_path_parsed()))
    {
        QPixmap thumbnail = img.scaled(100, 50, Qt::IgnoreAspectRatio, Qt::FastTransformation);
        ui->label_thumbnail->setPixmap(thumbnail);
        torrent_data.type = "img";
    }


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

    // check name because of torrent magnet
    if (ui->label_title->text().isEmpty()  && !m_torrent.name().isEmpty())
        ui->label_title->setText(m_torrent.name());

 //   qDebug() << "PROGRESS : " << m_torrent.progress();;

    float tmp = m_torrent.progress() * 100;
    //qDebug() << "PROGRESS float * 100 : " << m_torrent.name() << " : " << tmp;

    int progress = int(tmp + 0.5);
    ui->progressBar_torrent->setValue(progress);

    if (tmp >= 100) this->timer_get_torrent_progress->stop();
}