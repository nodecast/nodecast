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


#include "room.h"


Room::Room(Sphere_data a_sphere_data, QStackedWidget *parent) : sphere_data(a_sphere_data)
{
//    QGroupBox
//        QVBoxLayout
//            QScrollArea
//                scrollAreaWidgetContent
//                    QGraphicsView
//            QScrollArea
//                scrollAreaWidgetContent
//                    QTextEdit
//            QLineEdit

    m_room = NULL;

    my_nickname = Preferences().getNodecastLogin();
    my_login = Preferences().getNodecastAccount().value("login");


    refresh_users = new QTimer(this);

    groupBox = new QGroupBox;
    groupBox->setTitle("chat room : " + sphere_data.title);
    QFont font;
    font.setBold(true);
    font.setUnderline(true);
    groupBox->setFont(font);

    invit = new QPushButton;
    invit->setText("send invitation");
    connect(invit, SIGNAL(clicked()), this, SLOT(sendInvitation()));

    vbox = new QVBoxLayout;
    scrollArea_users = new QScrollArea;
    scrollArea_users->setWidgetResizable(true);
    scrollArea_chat = new QScrollArea;
    scrollArea_chat->setWidgetResizable(true);
    w_users_list = new QListWidget;
    chat_room = new QTextEdit;
    line_chat = new QLineEdit;

    connect(line_chat, SIGNAL(returnPressed()), this, SLOT(sendMessage()));

    scrollArea_users->setWidget(w_users_list);
    scrollArea_chat->setWidget(chat_room);
    vbox->addWidget(invit);
    vbox->addWidget(scrollArea_users);
    vbox->addWidget(scrollArea_chat);
    vbox->addWidget(line_chat);
    groupBox->setLayout(vbox);
    index_tab = parent->addWidget(groupBox);

    line_chat->setEnabled(false);


 //   connect(refresh_users, SIGNAL(timeout()), this, SLOT(refreshUsers()));
 //   refresh_users->start(1000);
}



Room::~Room()
{
    if (m_room && m_room->isJoined())
        m_room->leave("I'll be back");
}

void Room::sendInvitation()
{
    qDebug() << "send invitation";

    if (Xmpp_client::connection_failed)
    {
        errorMessageDialog = new QErrorMessage(this);
        errorMessageDialog->setModal(true);
        errorMessageDialog->showMessage("nodecast is not connected, check your account.");
        //errorMessageDialog->show();
        return;
    }


    room_invitation = new roominvit(this);
    connect(room_invitation, SIGNAL(emit_jids(QStringList)), this, SLOT(send_invitation(QStringList)));
    room_invitation->show();

}


void Room::send_invitation(QStringList jids)
{
    foreach(const QString jid, jids)
    {
        qDebug() << "SEND INVITATION ROOM : " << jid;
        m_room->sendInvitation(jid, "you have been invited to join this sphere, from " + Preferences().getNodecastLogin());
    }
}


void Room::sendMessage()
{
    QString message = line_chat->text();
    if (message.isEmpty()) return;

    if (message.startsWith("/msg"))
    {
        QStringList msg_dest = message.split(" ");
        qDebug() << "DEST : " << msg_dest << " SIZE : " << msg_dest.size();
        if (msg_dest.size() != 3)
        {
            line_chat->clear();
        }
        else
        {
            msg_dest.removeFirst();

            QString dest = msg_dest.takeFirst() + "@";

            QList<QListWidgetItem *> items = w_users_list->findItems(dest, Qt::MatchStartsWith );

            if (!items.isEmpty())
            {
                const QString dest_user = items.first()->text();
                const QString dest_message = msg_dest.takeLast();
                qDebug() << "USER : " << dest_user << " MSG : " << dest_message;

                Xmpp_client::instance()->sendMessage(dest_user, dest_message);
                line_chat->clear();
            }
        }
        return;
    }


    qDebug() << "Room::sendMessage : " << line_chat->text();

    m_room->sendMessage(line_chat->text());
    line_chat->clear();
}



void Room::receiveMessage(const QString from, const QString message)
{
    if (message.startsWith("/nodecast"))
        parseCommand(from, message);
    else
    {
        QString login = from.split("/").at(1);
        chat_room->append(login + " : " + message);
    }
}
void Room::parseCommand(const QString from, const QString message)
{
    if (message.contains("help"))
    {
        QString user = m_room->participantFullJid(from);
        qDebug() << "SEND HELP TO " << user;
      /*  Xmpp_client::instance()->sendMessage(user,
                                             "nodecast user list\n"
                                             "nodecast user get filenumber\n"
                                             "nodecast user getallfile\n"
                                             );
        */
    }
}

void Room::flushRoom()
{
    if (m_room)
    {
        m_room=NULL;
        qDebug() << "Room::flushRoom";
        w_users_list->clear();
        chat_room->clear();
        line_chat->setEnabled(false);
    }
}


void Room::setXMPPRoom(QXmppMucRoom* room)
{
    qDebug() << "Room::setXMPPRoom";
    if (m_room) m_room=NULL; // should be useless
    m_room = room;
    connect(m_room, SIGNAL(participantAdded(QString)), this, SLOT(newUser(QString)));
    connect(m_room, SIGNAL(participantRemoved(QString)), this, SLOT(delUser(QString)));
    line_chat->setEnabled(true);
}


void Room::joinXMPPRoom()
{
    qDebug() << "ROOM JOIN : " << m_room->name();
    if (m_room) m_room->join();
}


void Room::newUser(const QString &jid)
{
    qDebug() << "USER JOINED : " << jid;
    //QString l_jid = jid.split("/").at(1);
     QString l_jid = m_room->participantFullJid(jid);

     // sometimes participantFullJid return an empty string ... wtf.
     if (l_jid.isEmpty()) l_jid = jid;
     w_users_list->addItem(l_jid);
}


void Room::delUser(const QString &jid)
{
    qDebug() << "USER QUIT : " << jid;
    QString l_jid = m_room->participantFullJid(jid);
    if (l_jid.isEmpty()) l_jid = jid;
    qDebug() << "L JID : " << l_jid;
    QList<QListWidgetItem *> items = w_users_list->findItems(l_jid, Qt::MatchFixedString);

    if (!items.isEmpty())
        delete items.first();


    /*foreach(QListWidgetItem *item, items)
    {
        w_users_list->removeItemWidget(item);
        delete item;
    }*/


   // QString l_jid = jid.split("/").at(1);
   //     w_users_list->rem->addItem(l_jid);
}


void Room::refreshUsers()
{
    if (m_room && m_room->isJoined())
    {
        refresh_users->stop();
        refresh_users->deleteLater();
        l_users_list =  m_room->participants();

        foreach(QString user, l_users_list)
        {
            qDebug() << "FULL JID : " << m_room->participantFullJid(user);
            w_users_list->addItem(user.split("/").at(1));
        }
    }
}

QStringList Room::get_users()
{
    QString room_jid = m_room->jid();
    QStringList users =  m_room->participants();
    qDebug() << "USERS ROOM LIST : " << users;
    qDebug() << "MY NICK NAME : " << m_room->nickName();
    foreach(QString user, users)
    {
        QString tmp = user;
        tmp.replace(room_jid, "").replace("/", "");
        qDebug() << "TMP USER : " << tmp;
        if (tmp == m_room->nickName())
            users.removeAll(user);
    }
    qDebug() << "FULL JID LIST : " << users;
    return users;
}



