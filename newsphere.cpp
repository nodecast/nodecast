#include "newsphere.h"
#include "ui_newsphere.h"

newsphere::newsphere(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newsphere)
{
    ui->setupUi(this);


}

newsphere::~newsphere()
{
    delete ui;
}

void newsphere::on_buttonBox_accepted()
{
    qDebug() << "SPERE : " << ui->lineEdit_sphere->text();
    emit create_sphere(ui->lineEdit_sphere->text());
}
