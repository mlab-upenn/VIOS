#include "server.h"

void CloudServer::startServer(qint16 port)
{
    if(!this->listen(QHostAddress::Any,port))
        qDebug()<<"Cannot start Server in port" << port;
    else
        qDebug()<<"Listening at port" << port;
}

FaceServer::FaceServer(QObject *parent)
{
}

FaceServer::~FaceServer()
{
}

void FaceServer::incomingConnection(int socketDescriptor)
{
    qDebug() << socketDescriptor << "Connecting";
    FaceThread * newThread= new FaceThread(socketDescriptor,this);
    connect(newThread, SIGNAL(finished()),newThread,SLOT(deleteLater()));
    newThread->start();
}

ObjectServer::ObjectServer(QObject *parent)
{
}

ObjectServer::~ObjectServer()
{
}

void ObjectServer::incomingConnection(int socketDescriptor)
{
    qDebug() << socketDescriptor << "Connecting";
    ObjectThread * newThread= new ObjectThread(socketDescriptor,this);
    connect(newThread, SIGNAL(finished()),newThread,SLOT(deleteLater()));
    newThread->start();
}

SportsServer::SportsServer(QObject *parent)
{
}

SportsServer::~SportsServer()
{
}

void SportsServer::incomingConnection(int socketDescriptor)
{
    qDebug() << socketDescriptor << "Connecting";
    SportsThread * newThread= new SportsThread(socketDescriptor,this);
    connect(newThread, SIGNAL(finished()),newThread,SLOT(deleteLater()));
    newThread->start();
}
