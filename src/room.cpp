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
    refresh_users = new QTimer(this);

    groupBox = new QGroupBox;
    groupBox->setTitle("chat room : " + sphere_data.title);
    vbox = new QVBoxLayout;
    scrollArea_users = new QScrollArea;
    scrollArea_users->setWidgetResizable(true);
    scrollArea_chat = new QScrollArea;
    scrollArea_chat->setWidgetResizable(true);
    users_list = new QListWidget;
    chat_room = new QTextEdit;
    line_chat = new QLineEdit;

    scrollArea_users->setWidget(users_list);
    scrollArea_chat->setWidget(chat_room);
    vbox->addWidget(scrollArea_users);
    vbox->addWidget(scrollArea_chat);
    vbox->addWidget(line_chat);
    groupBox->setLayout(vbox);
    index_tab = parent->addWidget(groupBox);

    connect(refresh_users, SIGNAL(timeout()), this, SLOT(refreshUsers()));
    refresh_users->start(1000);
}



Room::~Room()
{
    if (m_room && m_room->isJoined())
        m_room->leave("I'll be back");
}


void Room::receiveMessage(QString message)
{
    chat_room->append(message);
}

void Room::refreshUsers()
{
    if (m_room && m_room->isJoined())
    {
        refresh_users->stop();
        refresh_users->deleteLater();

        QStringList users =  m_room->participants();
        //ROOM USERS :  ("test_3923f2dea95f460c8061c68728812331@conference.nodecast.net/fredix", "test_3923f2dea95f460c8061c68728812331@conference.nodecast.net/test")

        foreach(QString user, users)
        {
            users_list->addItem(user.split("/").at(1));
        }
    }
}
