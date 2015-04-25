#ifndef ROOMINVIT_H
#define ROOMINVIT_H

#include <QDialog>
#include <QDebug>

#include "xmpp_client.h"

namespace Ui {
class roominvit;
}

class roominvit : public QDialog
{
    Q_OBJECT

public:
    explicit roominvit(QWidget *parent = 0);
    ~roominvit();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::roominvit *ui;

signals:
    void emit_jids(QStringList jids);
};

#endif // ROOMINVIT_H
