#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QDialog>
#include <QDebug>
#include "preferences.h"

namespace Ui {
class account;
}

class account : public QDialog
{
    Q_OBJECT

public:
    explicit account(QWidget *parent = 0);
    ~account();

private slots:
    void on_buttonBox_accepted();

private:
    Preferences pref;
    Ui::account *ui;
};

#endif // ACCOUNT_H
