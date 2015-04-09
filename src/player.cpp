#include "player.h"

Player::Player(const QStringList &args, const QString &url, QWidget *parent) : QMPwidget(parent), m_url(url)
{    
    connect(this, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    QMPwidget::start(args);
}


void Player::stateChanged(int state)
{
    if (state == QMPwidget::NotStartedState) {
        qDebug("EXIT");
        //QApplication::exit();
    } else if (state == QMPwidget::PlayingState && mediaInfo().ok) {
        if (parentWidget()) {
            parentWidget()->resize(mediaInfo().size.width(), mediaInfo().size.height());
        } else {
            resize(mediaInfo().size.width(), mediaInfo().size.height());
        }
    }
}

void Player::showEvent(QShowEvent *event)
{
    if (!event->spontaneous() && state() == QMPwidget::IdleState) {
        QMPwidget::load(m_url);
    }
}

void Player::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_O) {
        QString url = QFileDialog::getOpenFileName(this, tr("Play media file..."));
        if (!url.isEmpty()) {
            m_url = url;
            QMPwidget::load(m_url);
        }
    } else {
        QMPwidget::keyPressEvent(event);
    }
}
