#include "openuritorrent.h"
#include "ui_openuritorrent.h"

Openuritorrent::Openuritorrent(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::openuritorrent)
{
    ui->setupUi(this);
}

Openuritorrent::~Openuritorrent()
{
    delete ui;
}

void Openuritorrent::on_buttonBox_accepted()
{
    QString link = ui->lineEdit->text();
    qDebug() << "URL : " << link;

    QTorrentHandle h = QBtSession::instance()->addMagnetUri(link);
    emit emit_handle(h);
}
