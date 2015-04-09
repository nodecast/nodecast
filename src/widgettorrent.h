#ifndef WIDGETTORRENT_H
#define WIDGETTORRENT_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QProcess>
#include <QTimer>

//#include "torrentpersistentdata.h"
#include "qtorrenthandle.h"
#include "qbtsession.h"

namespace Ui {
class widgettorrent;
}


class Widgettorrent : public QWidget
{
    Q_OBJECT

public:
    explicit Widgettorrent();
    ~Widgettorrent();    
//    void populate();
    static void populate(QString title, QLayout *parent = 0);
    static void unckeck_widget_selected(Widgettorrent *wt);


protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);


signals:
    void emit_title();

private:
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

public slots:
    void addTorrent(const QTorrentHandle &h);
};

#endif // WIDGETTORRENT_H
