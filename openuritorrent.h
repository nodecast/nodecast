#ifndef OPENURITORRENT_H
#define OPENURITORRENT_H

#include <QDialog>
#include <QDebug>
#include "qbtsession.h"

namespace Ui {
class openuritorrent;
}

class Openuritorrent : public QDialog
{
    Q_OBJECT

public:
    explicit Openuritorrent(QWidget *parent = 0);
    ~Openuritorrent();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::openuritorrent *ui;

signals:
    void emit_handle(QTorrentHandle handle);
};

#endif // OPENURITORRENT_H
