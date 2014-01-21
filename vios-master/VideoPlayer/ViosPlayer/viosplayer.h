#ifndef VIOSPLAYER_H
#define VIOSPLAYER_H

#include <QMainWindow>
#include <phonon/VideoPlayer>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QThread>
#include <QImage>
#include <QPixmap>
#include <vector>
#include <QTimer>
#include <QDataStream>
#include <QByteArray>
#include "wiiwrapper.h"
#include "dataparser.h"
#include "cloudclient.h"
#include <cstdlib>
#include <QSize>

using namespace Phonon;
using namespace std;

namespace Ui {
class ViosPlayer;
}

class ViosPlayer : public QMainWindow
{
    Q_OBJECT
private:
    VideoPlayer *player;
    QGraphicsView *overlayView;
    QGraphicsScene *overlayScene;
    QStackedLayout *display;
    QThread *mainThread, *wiiThread, *parserThread;
    QString homeBackground, screen, hostAdd,sendData;
    QMap< QString, QString> frameData,prevframeData;
    QImage homeImage;
    QString sportsUrl,prevsportsUrl;
    QTimer *timer;
    vector <QString> videoPath, videoChannel, XMLpath;
    vector <QRect> channelRect;
    vector <QSize> channelSize;
    WiiWrapper *wii;
    DataParser *parser;
    double hratio,vratio;
    int curChannel,hostPort,activeCount,curActive;
    bool showDetect,buttonOne;
    CloudClient *client;
    void highlightChannel();
    void playChannel();
    void serverType(int);

public:
    explicit ViosPlayer(QWidget *parent = 0);
    ~ViosPlayer();
public slots:
    void wiimoteHandler(const QString &);
private slots:
    void showHomeScreen();
    void processOverlay();
    void modifyOverlay();
    //void changeThread();
    
private:
    Ui::ViosPlayer *ui;
};

#endif // VIOSPLAYER_H
