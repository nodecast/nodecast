#ifndef PLAYER_H
#define PLAYER_H

#include <QFileDialog>
#include <QGridLayout>
#include <QShowEvent>
#include <QSlider>

//#include "qmpwidget.h"


// Custom player
class Player : public QObject
{
    Q_OBJECT

    public:
        Player(const QStringList &args, const QString &url, QWidget *parent = 0);

    private slots:
        void stateChanged(int state);

    protected:
        void showEvent(QShowEvent *event);
        void keyPressEvent(QKeyEvent *event);

    private:
        QString m_url;
};


#endif // PLAYER_H
