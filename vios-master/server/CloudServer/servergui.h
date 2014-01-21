#ifndef SERVERGUI_H
#define SERVERGUI_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QImage>
#include <QTextStream>
#include <QDebug>
#include <QSignalMapper>
#include <QDir>
#include <QObject>
#include <QPen>
#include <QGraphicsRectItem>
#include <QList>
#include <QRadioButton>
#include <QCheckBox>
#include <QTimer>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "server.h"

using namespace std;
using namespace cv;

namespace Ui {
class ServerGui;
}

class ServerGui : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ServerGui(QWidget *parent = 0);
    ~ServerGui(); 
    
private:
    Ui::ServerGui *ui;
    void configureUi();

    // Face Server
    FaceServer faceServer;

    // Object Server
    ObjectServer objectServer;

    // Sports Server
    SportsServer sportsServer;
    QString sportsTeam1, sportsTeam2;
    QGraphicsScene * sportsLogo1, *sportsLogo2;
    QImage sportsLogoImage1, sportsLogoImage2, sportsLogoOverlayImage1, sportsLogoOverlayImage2;

private slots:
    void faceShapeUpdate();
    void faceColorUpdate();
    void faceWidthUpdate(int x);
    void objectShapeUpdate();
    void objectColorUpdate();
    void objectWidthUpdate(int x);
};

#endif // SERVERGUI_H
