#ifndef NEWSPHERE_H
#define NEWSPHERE_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class newsphere;
}

class newsphere : public QDialog
{
    Q_OBJECT

public:
    explicit newsphere(QWidget *parent = 0);
    ~newsphere();

private slots:
    void on_buttonBox_accepted();

signals:
    void create_sphere(QString sphere);

private:
    Ui::newsphere *ui;
};

#endif // NEWSPHERE_H
