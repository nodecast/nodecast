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


#ifndef ROOM_H
#define ROOM_H

#include <QGroupBox>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTextEdit>
#include <QLineEdit>
#include <QStackedWidget>
#include <QListWidget>
#include <QTimer>
#include <QPushButton>
#include <QErrorMessage>

#include "xmpp_client.h"
#include "global.h"
#include "roominvit.h"

class Room : public QWidget
{
    Q_OBJECT

public:
    Room(Sphere_data a_sphere_data, QStackedWidget *parent = 0);
    ~Room();
    void receiveMessage(const QString from, const QString message);
    int index_tab;
    void setXMPPRoom(QXmppMucRoom* room);
    QStringList get_users();
    QString get_name() { return sphere_data.title;}
    void send_message(QString message)  {m_room->sendMessage(message);}

private:
    void parseCommand(const QString from, const QString message);

    QString my_nickname;
    QString my_login;

    QTimer *refresh_users;
    QGroupBox *groupBox;
    QVBoxLayout *vbox;
    QScrollArea *scrollArea_users;
    QScrollArea *scrollArea_chat;
    QTextEdit *chat_room;
    QLineEdit *line_chat;
    QListWidget *w_users_list;
    Sphere_data sphere_data;
    QXmppMucRoom* m_room;
    QStringList l_users_list;
    QPushButton *invit;
    roominvit *room_invitation;
    QErrorMessage *errorMessageDialog;

private slots:
    void refreshUsers();
    void newUser(const QString &jid);
    void delUser(const QString &jid);
    void sendMessage();
    void sendInvitation();
    void send_invitation(QStringList jids);
};

#endif // ROOM_H
