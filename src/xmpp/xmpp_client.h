#ifndef XMPP_CLIENT_H
#define XMPP_CLIENT_H

#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QInputDialog>

#include "QXmppMessage.h"
#include "QXmppLogger.h"
#include "QXmppClient.h"
#include <QXmppTransferManager.h>
#include <QXmppMucManager.h>
#include <QXmppRosterManager.h>
#include <QXmppVCardManager.h>
#include <QXmppUtils.h>
//#include <QXmppMessageReceiptManager.h>
#include <QHash>
#include <QMessageBox>
#include <QSplitter>
#include <QTextEdit>
#include <QGroupBox>
#include <QStackedWidget>
#include <QItemSelection>
#include <QMutex>

#include "preferences.h"
#include "xmlConsoleDialog.h"
#include "vCardCache.h"
#include "capabilitiesCache.h"
#include "utils.h"
#include "statusWidget.h"
#include "ui_roster.h"
#include "rosterItemModel.h"
#include "rosterItemSortFilterProxyModel.h"
#include "qbtsession.h"
#include "profileDialog.h"
#include "global_mutex.h"



struct File_data {
    QDir *file_dir=NULL;
    QString file_name="";
    QBuffer *buffer=NULL;
};




class Xmpp_client : public QXmppClient
{
    Q_OBJECT

public:
    Xmpp_client(QString m_login, QString m_password, int a_xmpp_client_port, QObject *parent = 0);
    ~Xmpp_client();
    static void connectXMPP();
    static void drop();
    static bool connection_failed;
    static Xmpp_client *instance();
    void connectToRoom(QString room_name);
    bool deleteRoom(QString room_name);

    void sendFile(const QString jid, const QString path);
    void addRoster(const QString jid);

    //QXmppMucRoom* get_room(QString room) { if (rooms.contains(room)) return rooms.value(room); else return NULL;}
    void show_xml_console();

    QSplitter* getRosterSplitter(){ return vRosterSplitter;}

private:
    void reload(QString login, QString password);
    void addPhotoHash(QXmppPresence&);
    void new_chat(const QString& bareJid);

    QWidget *roster_widget;
    QVBoxLayout *layoutRoster;
    QGroupBox *groupBoxContact;
    QStackedWidget *m_stacked_tab_chat;

    Ui::Roster *roster;
    static Xmpp_client* m_instance;
    QXmppTransferManager *transfer_manager;
    //QXmppMessageReceiptManager *receipt_manager;
    //QXmppRosterManager *roster_manager;
    Preferences prefs;
    QFile *log;
    QString m_login;
    QString m_password;
    int m_xmpp_client_port;
    QXmppLogger m_logger;
    QXmppPresence subscribe;
    QHash <QString, File_data> file_buffer;
    QString file_name;
    QXmppMucManager *muc_manager;
    QHash <QString, QXmppMucRoom*> rooms;
    xmlConsoleDialog m_consoleDlg;
    vCardCache *m_vCardCache;
    capabilitiesCache *m_capabilitiesCache;
    statusWidget m_statusWidget;
    QSplitter *vRosterSplitter;
    rosterItemModel *m_rosterItemModel;
    rosterItemSortFilterProxyModel *m_rosterItemSortFilterModel;
    QDir *file_dir;
    QString sphere_dest;
    QString file_extension;
    QHash <QString, int> itemMapChat;
    QHash <int, QTextEdit*> chatroomMap;
    QHash <int, QLineEdit*> linechatMap;


public slots:
    void init();
    void connectedToServer();
    void connectedError();
    void messageReceived(const QXmppMessage&);
    void presenceReceived(const QXmppPresence& presence);

private slots:
    void disconnectedToServer();
    void job_finished(QXmppTransferJob *job);
    void file_received(QXmppTransferJob *job);
    void job_error(QXmppTransferJob::Error error);
    void presenceChanged(const QString&, const QString&);
    void action_removeContact(const QString& bareJid);
    void statusTextChanged(const QString&);
    void filterChanged(const QString& filter);
    void rosterReceived();
    void rosterChanged(const QString& bareJid);
    void updateVCard(const QString& bareJid);
    void updateStatusWidget();
    void avatarChanged(const QImage&);
    void presenceStatusTypeChanged(QXmppPresence::AvailableStatusType);
    void presenceTypeChanged(QXmppPresence::Type);
    void showProfile(const QString& bareJid);
    void rosterItemSelectionChanged(const QItemSelection &selection);
    void sendMessageToJid();

    void action_addContact();

signals:
    void emit_connected(bool);
    void emit_chat(QString, QString);
    void emit_invitation(QString, QString, QString);
    void emit_room(QString room_name, QXmppMucRoom* room);
    void emit_progress_rawfile(qint64 done, qint64 total);
    void emit_receive_rawfile(QString sphere_dest, QString file_path, QXmppTransferJob *job);
};


#endif // XMPP_CLIENT_H
