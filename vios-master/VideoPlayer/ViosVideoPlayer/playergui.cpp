#include "playergui.h"
#include "ui_playergui.h"

PlayerGui::PlayerGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayerGui)
{
    ui->setupUi(this);

    movieVideo = "/home/rajeev/Dropbox/vios_team_use/demo videos/boondock.avi";
    player = new VideoPlayer(Phonon::VideoCategory, parent);
    //connect(player, SIGNAL(finished()), player, SLOT(deleteLater()));
    player->play(QString("/home/rajeev/Dropbox/vios_team_use/demo_videos/home.png"));
    player->show();
    view = new QGraphicsView(player->videoWidget());
    scene = new QGraphicsScene();
    view->showFullScreen();
    player->showFullScreen();

    /*
    QImage img(player->size(),QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&img);
    painter.setPen(QPen(QColor(255,0,0),3)); // use 1 to draw, the pen with is 1 pixel solid line
    painter.drawRect(QRect(20,20,200,200));
    painter.end();

    scene->clear();
    scene->addPixmap(QPixmap::fromImage(img));
    view->setScene(scene);
    view->setSceneRect(player->rect());
    view->setFixedSize(player->size());
    view->setFrameStyle( QFrame::NoFrame );
    view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setBackgroundRole( QPalette::Window );
    view->setStyleSheet("background:transparent");
    view->setAttribute(Qt::WA_TranslucentBackground);

    /*layout1 = new QVBoxLayout();
    layout2 = new QVBoxLayout();
    player->setLayout(layout1);
    view->setLayout(layout2);

    videoStack = new QStackedLayout();
    videoStack->addWidget(this->view);
    videoStack->addWidget(player);
    videoStack->setStackingMode(QStackedLayout::StackAll);
    setLayout(videoStack);*/
}

PlayerGui::~PlayerGui()
{
    delete ui;
}

