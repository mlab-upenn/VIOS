#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QDebug>
#include "thread.h"

class CloudServer : public QTcpServer
{
    Q_OBJECT
public:
    bool threadStarted;
    explicit CloudServer(QObject *parent = 0);
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
    FaceThread * faceThread;
    explicit FaceServer(QObject *parent = 0);
    ~FaceServer();
protected:
    void incomingConnection(int socketDescriptor);

};

class ObjectServer : public CloudServer
{
    Q_OBJECT
public:
    ObjectThread * objThread;
    explicit ObjectServer(QObject *parent = 0);
    ~ObjectServer();
protected:
    void incomingConnection(int socketDescriptor);

};

class SportsServer : public CloudServer
{
    Q_OBJECT
public:
    SportsThread * sportsThread;

    explicit SportsServer(QObject *parent = 0);
    ~SportsServer();
protected:
    void incomingConnection(int socketDescriptor);

};

#endif // SERVER_H
