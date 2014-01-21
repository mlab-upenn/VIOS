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

class Thread : public QThread
{
    Q_OBJECT
public:
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

public slots:
    virtual void readyRead();

};

class ObjectThread : public Thread
{
    Q_OBJECT
public:
    explicit ObjectThread(int ID, QObject *parent = 0);
    ~ObjectThread();
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
