#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QDebug>
#include <QTcpSocket>
#include <QImage>
#include <QImageReader>
#include <QDataStream>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QPen>
#include <QBuffer>
#include <opencv2/opencv.hpp>
#include <QGraphicsView>
#include <sstream>
#include <string>
#include <QNetworkRequest>
#include <QNetworkReply>
using namespace std;
using namespace cv;

class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(QObject *parent = 0);
    ~Thread();
    void run();
    QTcpSocket * socket;
    int socketDescriptor;
    QDataStream in;

signals:
    void error(QTcpSocket::SocketError socketError);

public slots:
    virtual void readyRead()=0;
    void disconnected();

};

class FaceThread : public Thread
{
    Q_OBJECT
public:
    explicit FaceThread(int ID, QObject *parent = 0);
    ~FaceThread();
public:
    QColor bboxColor;
    qreal bboxWidth;
    QString bboxShape;
public slots:
    virtual void readyRead();

};

class ObjectThread : public Thread
{
    Q_OBJECT
public:
    explicit ObjectThread(int ID, QObject *parent = 0);
    ~ObjectThread();

public:
    QColor bboxColor;
    qreal bboxWidth;
    QString bboxShape;

public slots:
    virtual void readyRead();

};

class SportsThread : public Thread
{
    Q_OBJECT
public:
    explicit SportsThread(int ID, QObject *parent = 0);
    ~SportsThread();

public slots:
    virtual void readyRead();

};
#endif // THREAD_H
