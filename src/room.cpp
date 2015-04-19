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


Room::Room(QString room_name, QStackedWidget *parent)
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


    groupBox = new QGroupBox;
    groupBox->setTitle("chat room : " + room_name);
    vbox = new QVBoxLayout;
    scrollArea_users = new QScrollArea;
    scrollArea_users->setWidgetResizable(true);
    scrollArea_chat = new QScrollArea;
    scrollArea_chat->setWidgetResizable(true);
    users = new QGraphicsView;
    chat_room = new QTextEdit;
    line_chat = new QLineEdit;

    scrollArea_users->setWidget(users);
    scrollArea_chat->setWidget(chat_room);
    vbox->addWidget(scrollArea_users);
    vbox->addWidget(scrollArea_chat);
    vbox->addWidget(line_chat);
    groupBox->setLayout(vbox);

    index_tab = parent->addWidget(groupBox);
}



Room::~Room()
{
}



void Room::receiveMessage(QString message)
{
    chat_room->append(message);
}
