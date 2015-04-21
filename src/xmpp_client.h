#ifndef XMPP_CLIENT_H
#define XMPP_CLIENT_H

#include <QBuffer>
#include <QFile>

#include "QXmppMessage.h"
#include "QXmppLogger.h"
#include "QXmppClient.h"
#include <QXmppTransferManager.h>
#include <QXmppMucManager.h>
#include <QHash>

#include "preferences.h"

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
    //QXmppMucRoom* get_room(QString room) { if (rooms.contains(room)) return rooms.value(room); else return NULL;}


private:
    static Xmpp_client* m_instance;
    Preferences prefs;
    QFile *log;
    QString m_login;
    QString m_password;
    int m_xmpp_client_port;
    QXmppLogger m_logger;
    QXmppPresence subscribe;
    QBuffer *m_buffer;
    QFile *file;
    QString file_name;
    QXmppMucManager *muc_manager;
    QHash <QString, QXmppMucRoom*> rooms;
    void reload(QString login, QString password);

public slots:
    void init();
    void connectedToServer();
    void connectedError();
    void messageReceived(const QXmppMessage&);
    void presenceReceived(const QXmppPresence& presence);

private slots:
    void job_finished();
    void file_received(QXmppTransferJob *job);
    void job_error(QXmppTransferJob::Error error);
    void job_progress(qint64 done, qint64 total);

signals:
    void emit_connected(bool);
    void emit_chat(QString, QString);
    void emit_room(QString room_name, QXmppMucRoom* room);
};


#endif // XMPP_CLIENT_H
