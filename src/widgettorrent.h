#ifndef WIDGETTORRENT_H
#define WIDGETTORRENT_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <QImageReader>
#include <QMimeDatabase>
#include <QDesktopServices>
#include <QMenu>
#include <QMessageBox>
#include <QXmppTransferManager.h>
#include <QMutexLocker>
#include <QRunnable>
#include <QThreadPool>

//#include "torrentpersistentdata.h"
#include "qtorrenthandle.h"
#include "qbtsession.h"
#include "global.h"
#include "iconprovider.h"
#include "deletionconfirmationdlg.h"
#include "flowlayout.h"
#include "preferences.h"
#include "global_mutex.h"

namespace Ui {
class widgettorrent;
}


struct Torrent_data {
    QString file="";
    QString filepath="";
    QString dirpath="";
    QString type="";
    qint64 size=0;
    QString hash="";
    QString extension="";
    bool is_torrent=true;
    QString transfer_sid="";
};


class updateThumbnail : public QObject, public QRunnable
{
    Q_OBJECT

public:
    updateThumbnail( Ui::widgettorrent *ui, Torrent_data *torrent_data, QTorrentHandle *torrent_handle);
    void run();

private:
    Torrent_data *m_torrent_data;
    QTorrentHandle *m_torrent_handle;
    Ui::widgettorrent *m_ui;
    QMimeDatabase m_mime_db;
};

class Widgettorrent : public QWidget
{
    Q_OBJECT

public:
    explicit Widgettorrent(Sphere_data a_sphere_data);
    ~Widgettorrent();    
    static int populate(Sphere_data a_sphere_data, FlowLayout *parent = 0);
    static void unckeck_widget_selected(Widgettorrent *wt);

    Sphere_data sphere_data;
    Torrent_data torrent_data;


protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);


signals:
    void emit_title();
    void emit_deleted(QWidget* wt);

private:
    void displayListMenuTorrent();
    void displayListMenuFile();
    QTorrentHandle m_torrent;
    Ui::widgettorrent *ui;
    QPixmap *myPix;
    QLabel m_title;
    QLabel m_thumbnail;

    int mFirstX;
    int mFirstY;

    QProcess *videoPlayer;
    QTimer *timer_get_torrent_progress;

private slots:
    void on_media_doubleClicked();
    void update_timer_torrent_progress();
    void update_torrent_type_thumbnail();

    void startSelectedTorrents();
    void pauseSelectedTorrents();
    void deleteSelectedTorrents();
    void openSelectedTorrentsFolder() const;
    void recheckSelectedTorrents();
    void job_progress(qint64 done, qint64 total);

public slots:
    void addTorrent(const QTorrentHandle &h);
    void addFile(const QFileInfo &file, QXmppTransferJob *job=NULL);
};






#endif // WIDGETTORRENT_H
