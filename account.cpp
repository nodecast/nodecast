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
    ui->lineEdit_token->setText(account.value("token"));

    show();
}

account::~account()
{
    delete ui;
}

void account::on_buttonBox_accepted()
{
    QHash<QString, QString> hash;


    hash["login"] = ui->lineEdit_login->text();
    hash["token"]  = ui->lineEdit_token->text();

    pref.setNodecastAccount(hash);

    qDebug() << "ACCOUNT : " << hash;

    //qDebug() << "LOGIN : " << ui->lineEdit_login->text();
    //qDebug() << "TOKEN : " << ui->lineEdit_token->text();
}
