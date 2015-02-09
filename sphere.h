#ifndef SPHERE_H
#define SPHERE_H

#include <QAbstractButton>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QColor>
#include <QtWebKitWidgets>

#include "flowlayout.h"
#include "widgettorrent.h"

enum class Sphere_scope { PRIVATE, PUBLIC, FIXED};


struct Sphere_data {
    QString title="";
    Sphere_scope scope;
    QString url="";
};



class Sphere : public QAbstractButton
{
    Q_OBJECT
public:
    Sphere(Sphere_data data, QWidget *parent = 0);
    ~Sphere();
    void addTorrent(const QTorrentHandle &h);
    virtual QSize sizeHint() const;
    QWidget *content;
    QSplitter *hSplitter;
    FlowLayout *flowLayout;

protected:
    virtual void paintEvent(QPaintEvent *e);
    //virtual void mousePressEvent(QMouseEvent * e);

private:
    QColor *m_color;
    static int index;
    int m_index;
    int mFirstX;
    int mFirstY;
    QString m_title;
    QWebView *view;
    QScrollArea *media_scroll;
    QWidget *media_container;
    Sphere_data sphere_data;

public slots:

private slots:
    void selected();

signals:
    void row(int);
};





#endif // SPHERE_H
