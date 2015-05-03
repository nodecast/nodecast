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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPushButton>
#include <QSettings>
#include <QDir>
#include <QProcess>
#include <QFileDialog>
#include <QThread>
#include <QTimer>
#include <QPainter>
#include <QRadioButton>
#include <QScrollArea>
#include <QList>
#include <QIcon>
#include <QStaticText>
#include <QStackedWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHash>
#include <QSystemTrayIcon>
#include <QMenu>

#include "video.h"
#include "preferences.h"
#include "qbtsession.h"
#include "openuritorrent.h"
#include "downloadfromurldlg.h"
#include "widgettorrent.h"
#include "options_imp.h"
#include "transferlistfilterswidget.h"
#include "torrentpersistentdata.h"
#include "flowlayout.h"
#include "about_imp.h"
#include "account.h"
#include "newsphere.h"
#include "godcast_api.h"
#include "sphere.h"
#include "room.h"
#include "scannedfoldersmodel.h"

#ifdef Q_OS_MAC
void fixNativeWindow( QMainWindow *window );
void minaturize( QMainWindow *window );
#endif


namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

public slots:


protected slots:
    void loadPreferences(bool configure_session=true);
    void optionsSaved();
    void on_actionAbout_triggered();
    void processDownloadedFiles(QString path, QString url);
    void downloadFromURLList(const QStringList& urls);



private slots:
    void create_sphere(QString sphere_name);
    void addTorrent(const QTorrentHandle &h);

    void shutdownCleanUp();

    void on_actionQuit_triggered();

    void on_actionPreferences_triggered();

    void on_pushButton_clicked();

    void on_listWidget_media_doubleClicked(const QModelIndex &index);

    void on_pushButton_play_clicked();

    void launch_timer_handle(QTorrentHandle h);
    void update_timer_torrent_progress();


    void on_pushButton_spherenew_clicked();


    void on_media_doubleClicked(QString title);


    void on_actionAccount_triggered();

    void refresh_spheres(QVariantMap sphere);


    void changePage(int current);
    void XmppChangeConnectionStatus(bool status);
    void NatChangeConnectionStatus(bool status);

    void receiveMessageChat(QString from, QString message);
    void receiveInvitation(QString invitation, QString reason);

    void on_actionTransferts_triggered();
    void mapRoom(QString room_name, QXmppMucRoom *room);

    void on_actionXml_console_triggered();
    void action_trayIconActivated(QSystemTrayIcon::ActivationReason reason);


private:
    void createTrayIconAndMenu();
    void populate();
    void load_spheres();
    void create_sphere(QString sphere_name, QString sphere_dir);

    ScanFoldersModel *m_scanFolders;

    QStackedWidget *m_stacked_tab_medias;
    QStackedWidget *m_stacked_tab_room;

    QPointer<about> aboutDlg;
    QPointer<newsphere> new_sphere;

    QPointer<account> accountDlg;
    QScrollArea *media_scroll;
    QWidget *media_container;
   // FlowLayout *flowLayout;

    QList <Widgettorrent*> list_torrents;

    QPointer<options_imp> options;
    TransferListWidget *transferList;
    TransferListFiltersWidget *transferListFilters;
    QSplitter *hSplitter;
    QSplitter *vSplitter;

    QHash<QString, Sphere *> m_spheres_private;
    QHash<QString, Sphere *> m_spheres_public;

    QHash<int, Sphere *> sphere_tab;

    QThread *thread_torrent;
    QTimer *timer_get_torrent_progress;
    QTimer *timer_set_torrent_progress;

    QSettings settings;
    Ui::MainWindow *ui;
    Video video;
    Preferences prefs;
    QProcess *videoPlayer;
    Openuritorrent openuritorrent;
    int torrent_index;
    QTorrentHandle m_torrent;
    QPointer<downloadFromURL> downloadFromURLDialog;

   QPointer<Widgettorrent> widgettorrentThumbnail;

   Godcast_api *m_godcastapi;
   QAction m_quitAction;
#ifndef QT_NO_SYSTEMTRAYICON
    QSystemTrayIcon m_trayIcon;
    QMenu m_trayIconMenu;
#endif
};

#endif // MAINWINDOW_H
