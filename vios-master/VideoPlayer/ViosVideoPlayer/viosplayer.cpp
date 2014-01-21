#include "viosplayer.h"

ViosPlayer::ViosPlayer(Phonon::Category category, QWidget *parent) :
    Phonon::VideoPlayer(category, parent)
{
    view = new QGraphicsView(this->videoWidget());
    scene = new QGraphicsScene();
    //p = view->palette();
    //p.setColor(QPalette::Window,Qt::transparent);
    //view->setPalette(p);
    view->setFrameStyle( QFrame::NoFrame );
    view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setBackgroundRole( QPalette::Window );
    videoStack = new QStackedLayout();
    videoStack->setStackingMode(QStackedLayout::StackAll);
    videoStack->addWidget(this->videoWidget());
    videoStack->addWidget(this->view);
    //videoStack->show();
}

void ViosPlayer::paintEvent(QPaintEvent *){
    qDebug() << "I am here";
}

void ViosPlayer::mousePressEvent(QMouseEvent *){
    pause();
    videoStack->update();
    view->setScene(scene);
    view->scene()->setSceneRect(this->videoWidget()->rect());
    qDebug() << this->videoWidget()->rect();
    view->setFixedSize( this->videoWidget()->size());
    scene->addRect(10,10,20,20,QPen(QColor(255,0,0),3));
    //view->show();
    //videoStack->show();
}
