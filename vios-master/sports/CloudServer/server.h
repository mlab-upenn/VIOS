#ifndef SERVER_H
#define SERVER_H

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
#include <QTcpServer>
#include <QDebug>
#include "thread.h"

class CloudServer : public QTcpServer
{
    Q_OBJECT
public:
    void startServer(qint16 port);

signals:

public slots:

protected:
    virtual void incomingConnection(int socketDescriptor)=0;

};

class FaceServer : public CloudServer
{
    Q_OBJECT
public:
    explicit FaceServer(QObject *parent = 0);
    ~FaceServer();
protected:
    void incomingConnection(int socketDescriptor);

};

class ObjectServer : public CloudServer
{
    Q_OBJECT
public:
    explicit ObjectServer(QObject *parent = 0);
    ~ObjectServer();
protected:
    void incomingConnection(int socketDescriptor);

};

class SportsServer : public CloudServer
{
    Q_OBJECT
public:
    explicit SportsServer(QObject *parent = 0);
    ~SportsServer();
protected:
    void incomingConnection(int socketDescriptor);

};

#endif // SERVER_H
