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


#ifndef SPHERE_H
#define SPHERE_H

#include <QAbstractButton>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QColor>
#include <QtWebKitWidgets>
#include <QUuid>

#include "global.h"
#include "flowlayout.h"
#include "widgettorrent.h"
#include "torrentcreator/torrentcreatordlg.h"
#include "room.h"
#include "filesystemwatcher.h"
#include "scannedfoldersmodel.h"

class Sphere : public QAbstractButton
{
    Q_OBJECT
public:
    Sphere(Sphere_data data, QStackedWidget *stacked_room, QStackedWidget *parent = 0);
    ~Sphere();
    static QString gen_directory(QString sphere_name);
    void addTorrent(const QTorrentHandle &h);
    void addFile(const QString &file_path, QXmppTransferJob *job);
    void populate();
    virtual QSize sizeHint() const;
    int index_tab;
    bool isScopeFixed();
    bool isScopePrivate();
    bool isScopePublic();
    void reloadWeb();
    QString getTitle() {return sphere_data.title;}
    QString getDirectory() {return sphere_data.directory;}
    void connectRoom(QXmppMucRoom *room);
    void joinRoom();
    void flushRoom();
    void receive_message(const QString login, const QString message);
    int getRoomIndex();
    QGroupBox * getRoomWidget();

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void dropEvent(QDropEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void mousePressEvent(QMouseEvent * e);

private:
    void displayListMenu();
    void sendXmppFile(QString path_file);

    QStackedWidget *m_stacked_room;
    FlowLayout *flowLayout;
    QSplitter *hSplitter;
    QScrollArea *media_scroll;
    QWidget *content;

    QColor *m_color;
    static int index;
    int m_index;
    int mFirstX;
    int mFirstY;
    QString m_title;
    QWebView *view;
    QWidget *media_container;
    Sphere_data sphere_data;
    QPointer<TorrentCreatorDlg> createTorrentDlg;
    Room *m_room;
    ScanFoldersModel *m_scanFolders;
    int nb_media;


    QWidget *widget_container;
    QVBoxLayout *vbox;
    QWidget *widget;
    QHBoxLayout *hbox;
    QLabel *filter_label;
    QToolButton *toolButton_video;
    QToolButton *toolButton_image;
    QToolButton *toolButton_audio;
    QLabel *media_label;
    QLabel *media_label_counter;
    QLabel *upload_label;
    QLabel *upload_label_counter;
    QLabel *download_label;
    QLabel *download_label_counter;


public slots:

private slots:
    void addTorrent(QString path, bool fromScandir);
    void torrentsAdded(QStringList &torrents);

    void leaveSelectedSphere();
    void joinSelectedSphere();
    void deleteSelectedSphere();
    void openSelectedSphereFolder() const;
    void renameSelectedSphere();
    void deleted_torrent();

signals:
    void row(int);
    void emit_deleted(Sphere*);
};





#endif // SPHERE_H
