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
