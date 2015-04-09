#ifndef QTRACKERSERV_H
#define QTRACKERSERV_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QTcpServer>

#include "qtracker.h"



class Qtrackerserv : public QTcpServer
{
    Q_OBJECT
public:
    explicit Qtrackerserv(const quint16 port, QObject *parent = 0);
    ~Qtrackerserv();
//    Scf *scf;

private:
    QThread *t_scf;

protected:
    void incomingConnection(qintptr socketDescriptor);

};




#endif // QTRACKERSERV_H
