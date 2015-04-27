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
    if (line_chat->text().isEmpty()) return;

    qDebug() << "Room::sendMessage : " << line_chat->text();

    m_room->sendMessage(line_chat->text());
    line_chat->clear();
}



void Room::receiveMessage(QString message)
{
    chat_room->append(message);
}

void Room::setXMPPRoom(QXmppMucRoom* room)
{
    m_room = room;

    connect(m_room, SIGNAL(participantAdded(QString)), this, SLOT(newUser(QString)));
    connect(m_room, SIGNAL(participantRemoved(QString)), this, SLOT(delUser(QString)));


}

void Room::newUser(const QString &jid)
{
    qDebug() << "USER JOINED : " << jid;
    //QString l_jid = jid.split("/").at(1);
     QString l_jid = m_room->participantFullJid(jid);
        w_users_list->addItem(l_jid);
}


void Room::delUser(const QString &jid)
{
    qDebug() << "USER QUIT : " << jid;
    const QString l_jid = m_room->participantFullJid(jid);
    qDebug() << "L JID : " << l_jid;
    QList<QListWidgetItem *> items = w_users_list->findItems(l_jid, Qt::MatchFixedString);

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
    QStringList users =  m_room->participants();
    qDebug() << "USERS ROOM LIST : " << users;
    QStringList users_fulljid;
    foreach(QString user, users)
    {
        if (user.split("/").takeLast() != my_nickname)
            users_fulljid << m_room->participantFullJid(user);
    }
    qDebug() << "FULL JID LIST : " << users_fulljid;
    return users_fulljid;
}



