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


void Widgettorrent::populate(QLayout *parent)
{

    std::vector<libtorrent::torrent_handle> torrents = QBtSession::instance()->getSession()->get_torrents();

    std::vector<libtorrent::torrent_handle>::const_iterator it = torrents.begin();
    std::vector<libtorrent::torrent_handle>::const_iterator itend = torrents.end();
    for ( ; it != itend; ++it)
    {
        const QTorrentHandle h(*it);
        //    if (HiddenData::hasData(h.hash()))
        //      continue;
        Widgettorrent *wt = new Widgettorrent();
        parent->addWidget(wt);
        qDebug() << "Widgettorrent::populate";
        wt->addTorrent(h);
        QObject::connect(QBtSession::instance(), SIGNAL(addedTorrent(QTorrentHandle)), wt, SLOT(addTorrent(QTorrentHandle)));
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
    arguments << "-a" << Preferences().getVideoPlayer() << Preferences().getSavePath() + "/" + m_torrent.name();
#else
    program = Preferences().getVideoPlayer();
    arguments << Preferences().getSavePath() + "/" + m_torrent.name();
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

    connect(this, SIGNAL(emit_title()), this, SLOT(on_media_doubleClicked()));

    m_torrent = h;
    m_title.setText(h.name());
    ui->label_title->setText(h.name());


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
