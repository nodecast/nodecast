#include "roominvit.h"
#include "ui_roominvit.h"

roominvit::roominvit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::roominvit)
{
    ui->setupUi(this);
}

roominvit::~roominvit()
{
    delete ui;
}

void roominvit::on_buttonBox_accepted()
{
    QStringList jids = ui->plainTextEdit_jid->toPlainText().split("\n");
    qDebug() << "JIDS : " << jids;
    emit emit_jids(jids);
}
