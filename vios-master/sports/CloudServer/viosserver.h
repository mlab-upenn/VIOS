#ifndef VIOSSERVER_H
#define VIOSSERVER_H

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
#include <QGraphicsScene>
#include <QTimer>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "server.h"

using namespace cv;
using namespace std;

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
};

#endif // VIOSSERVER_H
