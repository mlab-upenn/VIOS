#ifndef PLAYERGUI_H
#define PLAYERGUI_H

#include <QMainWindow>
#include <QMoveEvent>
#include <QWebView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include "viosplayer.h"

namespace Ui {
class PlayerGui;
}

class PlayerGui : public QMainWindow
{
    Q_OBJECT
private:
    QString movieVideo,sportsVideo,facerecVideo, objdetVideo;
    VideoPlayer *player;
    QStackedLayout *videoStack;
    QGraphicsView *view;
    QGraphicsScene *scene;
    QVBoxLayout *layout1,*layout2;

public:
    explicit PlayerGui(QWidget *parent = 0);
    ~PlayerGui();
    
private:
    Ui::PlayerGui *ui;
};

#endif // PLAYERGUI_H
