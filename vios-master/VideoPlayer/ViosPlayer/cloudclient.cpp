#include "cloudclient.h"

CloudClient::CloudClient(QObject * parent)
{

}

void CloudClient::connectToServer(QString host, qint16 port){
    socket = new QTcpSocket();
    in.setDevice(socket);
    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()),Qt::DirectConnection);
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()),Qt::DirectConnection);
    socket->connectToHost(host,port);
    socket->waitForConnected(2000);
    qDebug() << "Connected to Server";
}

void CloudClient::sendToServer(QString & data){
    QByteArray outData;
    QDataStream out(&outData,QIODevice::WriteOnly);
    outData.clear();
    out << data;
    socket->write(outData);
    qDebug()<<data;
    qDebug() << "Sent Data to Server";
}

void CloudClient::receiveFromServer(QImage & image){
    /*inData = inData.right(inData.size()-4);
    inData = qUncompress(inData);
    qDebug() << "QByteArray Uncompress input Size : " << inData.size();
    QBuffer buffer(&inData);
    image.load(&buffer, "PNG");
    qDebug() << "Image Size : " << image.size();
    //emit receivedInfo();*/
}

void CloudClient::disconnectFromServer(){
    socket->close();
    qDebug() << "Disconnected from Server";
}

void CloudClient::readyRead(){
    static qint64 byteCount = 0;
    QByteArray tmp;
    qDebug() << "socket byteAvailable : " << socket->bytesAvailable();
    /*if (byteCount == 0){
        byteCount += (qint64)socket->bytesAvailable() - sizeof(qint64);
        in >> packetByteCount;
        tmp = socket->readAll();
        qDebug() << "packetByteCount : " << packetByteCount;
        inData = NULL;
    }
    else{
        byteCount += (qint64)socket->bytesAvailable();
        tmp = socket->readAll();
    }
    inData += tmp;*/
    //inData = NULL;
    //inData = socket->readAll();
    QString searchString;
    in>>searchString;
    istringstream liness;
    liness.str(searchString.toStdString());
    qDebug() << "Server info string: "<<searchString;
    string shap;
    int r,g,b,width;
    liness>>r>>g>>b>>width>>shap;
    qDebug() << "Server Info :" << r << g << b << width << QString::fromStdString(shap);
    col = QColor(r,g,b);
    lineWidth = width;
    this->shape = QString::fromStdString(shap);
    /*qDebug() << "Current byteCount : " << byteCount;
    qDebug() << "inData QByteArray size: " << inData.size();
    if (byteCount >= packetByteCount){
        qDebug() << "inData Size : " << inData.size();
        receiveFromServer(overlayImage);

        byteCount = 0;
        packetByteCount = 0;
    }*/
}

void CloudClient::disconnected()
{
    qDebug() << "Disconnected: ";
    socket->deleteLater();
}
