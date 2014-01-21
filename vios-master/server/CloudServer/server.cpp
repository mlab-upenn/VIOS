#include "server.h"

CloudServer::CloudServer(QObject *parent) :
    QTcpServer(parent)
{
    threadStarted = 0;
}

void CloudServer::startServer(qint16 port)
{
    if(!this->listen(QHostAddress::Any,port))
        qDebug()<<"Cannot start Server in port" << port;
    else
        qDebug()<<"Listening at port" << port;
}

FaceServer::FaceServer(QObject *parent) :
    CloudServer(parent)
{
}

FaceServer::~FaceServer()
{
    threadStarted = 0;
}

void FaceServer::incomingConnection(int socketDescriptor)
{
    qDebug() << socketDescriptor << "Connecting";
    qDebug() << "face server"<<endl;
    faceThread= new FaceThread(socketDescriptor,this);
    connect(faceThread, SIGNAL(finished()),faceThread,SLOT(deleteLater()));
    faceThread->start();
    threadStarted = 1;
}

ObjectServer::ObjectServer(QObject *parent) :
    CloudServer(parent)
{
}

ObjectServer::~ObjectServer()
{
    threadStarted = 0;
}

void ObjectServer::incomingConnection(int socketDescriptor)
{
    qDebug() << socketDescriptor << "Connecting";
    qDebug() << "obj server"<<endl;
    objThread= new ObjectThread(socketDescriptor,this);
    connect(objThread, SIGNAL(finished()),objThread,SLOT(deleteLater()));
    objThread->start();
    threadStarted = 1;
}

SportsServer::SportsServer(QObject *parent) :
    CloudServer(parent)
{

}

SportsServer::~SportsServer()
{
    threadStarted = 0;
}

void SportsServer::incomingConnection(int socketDescriptor)
{
    qDebug() << socketDescriptor << "Connecting";
    qDebug() << "sports server"<<endl;
    sportsThread= new SportsThread(socketDescriptor,this);
    connect(sportsThread, SIGNAL(finished()),sportsThread,SLOT(deleteLater()));
    sportsThread->start();
    threadStarted = 1;
}
