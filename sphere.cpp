#include "sphere.h"


int Sphere::index = 0;

Sphere::Sphere(Sphere_data data, QStackedWidget *parent)
        : QAbstractButton(parent), sphere_data(data)
{

//    QPalette pal = palette();
//    pal.setColor(QPalette::Background, Qt::black);
//    setPalette(pal);
//    setAutoFillBackground(true);

  //  data.title.isEmpty()? m_title = data.title : m_title = "";

   // content = new QWidget(parent);


    switch(sphere_data.scope)
    {
    case Sphere_scope::PRIVATE :
        m_color = new QColor(105,105,105);

        media_scroll = new QScrollArea();
        media_scroll->setWidgetResizable(true);
        media_scroll->setFrameStyle(QFrame::NoFrame);
        media_container = new QWidget(media_scroll);
        flowLayout = new FlowLayout(media_container);

        media_scroll->setWidget(media_container);
        index_tab = parent->addWidget(media_scroll);
        qDebug() << "INDEX TAB : " << index_tab;

        break;

    case Sphere_scope::PUBLIC :
        m_color = new QColor(119,136,153);

        media_scroll = new QScrollArea();
        media_scroll->setWidgetResizable(true);
        media_scroll->setFrameStyle(QFrame::NoFrame);
        media_container = new QWidget(media_scroll);
        flowLayout = new FlowLayout(media_container);

        media_scroll->setWidget(media_container);

        view = new QWebView();
        view->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
        view->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
        view->settings()->setAttribute(QWebSettings::ZoomTextOnly, true);
        view->settings()->setAttribute(QWebSettings::AutoLoadImages, true);
        view->settings()->setAttribute(QWebSettings::NotificationsEnabled, false);
        view->setZoomFactor(0.8);
        view->load(QUrl(sphere_data.url));


        hSplitter = new QSplitter(Qt::Vertical);
        hSplitter->addWidget(view);
        hSplitter->addWidget(media_scroll);

        index_tab = parent->addWidget(hSplitter);
        qDebug() << "INDEX TAB : " << index_tab;

        break;

    case Sphere_scope::FIXED :
        m_color = new QColor(100, 143, 000);
        content = new QWidget();

        index_tab = parent->addWidget(content);
        qDebug() << "INDEX TAB : " << index_tab;

        break;
    }


//    if (scope.compare("private") == 0)
//        m_color = new QColor(105,105,105);
//    else if (scope.compare("public") == 0)
//        m_color = new QColor(119,136,153);
//    else
//        m_color = new QColor(100, 143, 000);


    setAutoExclusive(true);
    setCheckable(true);

    m_index = index++;

    connect(this, SIGNAL(clicked()), this, SLOT(selected()));
//    initStyleOption(QStyleOptionButton::Flat);
}

QSize Sphere::sizeHint() const
{
    return QSize(105,105);
}



Sphere::~Sphere()
{}



void Sphere::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
//    QString css = "background-color: transparent; border: none; color: white";
//    setStyleSheet(css);


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);


    QColor color = this->isChecked() ? QColor(192,192,192) : *m_color;


    //painter.setPen( QColor("#222326") );
    //painter.setOpacity(1.0);
    //painter.setPen( Qt::TransparentMode );
    painter.setBrush(color);
    painter.drawEllipse(0, 0, 100, 100);
    //painter.setPen( Qt::SolidLine );

    //painter.drawText(10, 50, m_title);

    //painter.drawText(60, 70, m_title);

    QRectF rect(10, 10, 80, 90);
    painter.drawText(rect, Qt::AlignCenter, sphere_data.title);



//    painter.drawStaticText(10, 50, QStaticText("<center>" + m_title + "</center>"));


  //  setScaledContents(false);
}


void Sphere::selected()
{
    this->setChecked(true);
    //this->update();
    qDebug() << "selected : " << sphere_data.title << " index : " << m_index;
    emit row(m_index);
}

//void Sphere::mousePressEvent(QMouseEvent *e)
//{
//    mFirstX=0;
//    mFirstY=0;
////    mFirstClick=true;
////    mpaintflag=false;


//    if(e->button() == Qt::LeftButton)
//    {
//        qDebug() << "SPHERE CLICKED";
//        m_color = QColor(105,105,105);
//        this->setChecked(true);
//        //this->update();

//                //store 1st point
//              //  if(mFirstClick)
//              //  {
//                    mFirstX = e->x();
//                    mFirstY = e->y();
//                  //  mFirstClick = false;
//                  //  mpaintflag = true;
//                    qDebug() << "First image's coordinates" << mFirstX << "," << mFirstY << " title " << m_title;
//                  //  update();

//                //}

//            }
//}



void Sphere::addTorrent(const QTorrentHandle &h)
{
    Widgettorrent *wt = new Widgettorrent();
    wt->addTorrent(h);
    flowLayout->addWidget(wt);
}



void Sphere::populate()
{
    qDebug() << "Sphere::polulate";
    Widgettorrent::populate(flowLayout);
}
