#ifndef GODCAST_API_H
#define GODCAST_API_H

#include <QObject>
#include <QtNetwork>
#include <QThread>
#include <QJsonDocument>
#include <QMessageBox>
#include <QMutex>



class Godcast_api : public QObject
{
    Q_OBJECT

public:
    explicit Godcast_api(QString a_server, QString m_http_port, QObject *parent = 0);
    ~Godcast_api();
    void users_get(QString admin_token);
    void user_post(QString admin_token, QString json);

    void spheres_get(QString user_token);
    void sphere_post(QString user_token, QString spherename);
    void sphere_delete(QString user_token, QString sphere_id);
    void workflows_get(QString admin_token);
    void workflow_post(QString admin_token, QString workflow_name, QString owner_email, QString json);
    void workflow_delete(QString admin_token);

    QString m_credentials;
    QString m_filename;
    QString m_server;
    QString m_user_email;
    QString m_user_password;

    QString m_http_port;
    QString m_sphere_uuid;
    QString m_node_password;
    QString m_push_method;
    QNetworkAccessManager *m_network;


public slots:
    void slotRequestFinished(QNetworkReply *);
    void replyError(QNetworkReply::NetworkError errorCode);

private slots:
    void init();
    void session_get(QString uuid);

signals:
    void api_error(QString error);
    void httpResponse(int http_error);
    void users_Changed(QVariantMap user);
    void sphere_Changed(QVariantMap node);
    void workflows_Changed(QVariantMap workflow);
    void workflowstatus_Changed(QString status);

    void nodestatus_Changed(QString status);
    void get_Token(QString token);

private:
    QMutex *m_mutex;
    void payload_push();
    QString session_uuid;
    QTimer *session_get_timer;
    QByteArray *m_content;
    QNetworkReply *m_reply;
    QUrl url_get;
    QUrl url_push;
    QNetworkRequest m_request;
    QAuthenticator *m_auth;
    QString m_post_response;
    int m_http_error;
};



#endif // GODCAST_API_H
