#ifndef VIOSPLAYER_H
#define VIOSPLAYER_H

#include <VideoPlayer>
#include <QPainter>
#include <QStackedLayout>
#include <QStackedWidget>
#include <VideoWidget>
#include <QGraphicsScene>
#include <QGraphicsView>

using namespace Phonon;

class ViosPlayer : public Phonon::VideoPlayer
{
    Q_OBJECT
private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    QStackedLayout *videoStack;
    QPalette p;

public:
    explicit ViosPlayer(Phonon::Category category, QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

signals:
    
public slots:
    
};

#endif // VIOSPLAYER_H
