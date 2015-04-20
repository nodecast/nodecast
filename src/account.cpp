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

#include "account.h"
#include "ui_account.h"

account::account(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::account)
{
    ui->setupUi(this);
    QHash<QString, QString> account;
    account = pref.getNodecastAccount();

    ui->lineEdit_login->setText(account.value("login") );
    ui->lineEdit_password->setText(account.value("password"));

    show();
}

account::~account()
{
    qDebug() << "DELETE ACCOUNT";
    m_xmpp_client->deleteLater();
    delete ui;
}

void account::on_buttonBox_accepted()
{
    QHash<QString, QString> hash;


    hash["login"] = ui->lineEdit_login->text();
    hash["password"]  = ui->lineEdit_password->text();

    pref.setNodecastAccount(hash);
    pref.sync();

    qDebug() << "ACCOUNT : " << hash;


    Xmpp_client::connectXMPP();

    //thread_xmpp_client = new QThread(this);

//    if (Xmpp_client::instance == 0)
//    {
//        qDebug() << "XMPP BEFORE";
//        m_xmpp_client = new Xmpp_client(hash["login"], hash["password"], 5223);
//        connect(m_xmpp_client, SIGNAL(emit_tchat(QString)), SLOT(receive_tchat(QString)));
//        qDebug() << "XMPP AFTER";
//    }
//    else qDebug() << "XMPP ALREADY CONNECTED";


    //connect(m_xmpp_client, SIGNAL(destroyed()), thread_xmpp_client, SLOT(quit()), Qt::DirectConnection);
    //connect(thread_xmpp_client, SIGNAL(started()), m_xmpp_client, SLOT(init()));
    //connect(ncw, SIGNAL(parseClientData(QString)), m_xmpp_client, SLOT(receive_payload(QString)), Qt::QueuedConnection);


    //qDebug() << "LOGIN : " << ui->lineEdit_login->text();
    //qDebug() << "TOKEN : " << ui->lineEdit_token->text();
}

void account::receive_tchat(QString message)
{
    qDebug() << "RECEIVE message : " << message;

}
