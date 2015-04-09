#include "video.h"
#include "ui_video.h"

Video::Video(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Video)
{
    ui->setupUi(this);


//    return;





     //QWidget widget(video->widget_video);


     qDebug("1");

//     QGridLayout layout(&widget);
//     widget.setLayout(&layout);






    // QGridLayout layout(ui->widget_video);
     //ui->widget_video->setLayout(&layout);


     //QSlider seekSlider(Qt::Horizontal, ui->widget_video);
     //QSlider volumeSlider(Qt::Horizontal, ui->widget_video);
     //volumeSlider.setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));

     qDebug("2");
     //QString url = "/home/fred/tmp/Hackers.Les.Gardiens.du.Nouveau.Monde.mp4";

     QStringList args;
     //Player player(args, "/home/fred/tmp/Hackers.Les.Gardiens.du.Nouveau.Monde.mp4", ui->widget_video);
     //player = new Player(args, "/home/fred/data/nodecast/Hackers.Les.Gardiens.du.Nouveau.Monde.mp4", ui->widget_video);

     qDebug("3");

//     player.setSeekSlider(&seekSlider);
  //   player.setVolumeSlider(&volumeSlider);

//     player->setSeekSlider(ui->horizontalSlider_video);
//     player->setVolumeSlider(ui->horizontalSlider_volume);


     //layout.addWidget(&player, 0, 0, 1, 2);
     //layout.addWidget(&seekSlider, 1, 0);
     //layout.addWidget(&volumeSlider, 1, 1);
     //ui->widget_video->show();
     //Ui::Video.widget_video->show();



}

Video::~Video()
{
    qDebug("VIDEO END");

    //player->stop();
    //delete player;
    delete ui;
}
