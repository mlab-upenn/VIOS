#include "thread.h"

Thread::Thread(QObject *parent) :
    QThread(parent)
{
}

Thread::~Thread(){
    delete socket;
    socket = NULL;
}

void Thread::run()
{
    qDebug() << socketDescriptor << "Spawning new thread";
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }
    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_8);
    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()),Qt::DirectConnection);
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()),Qt::DirectConnection);
    qDebug() << socketDescriptor << "Client Connected";
    if (socket->localPort() == 1200){
        qDebug()<<"requesting URl";
        system( "wget --quiet http://spectrackulo.us/episode.php?episode_name=The\\ Big\\ Bang\\ Theory");
    }
    else if (socket->localPort() == 1201){
        system( "wget --quiet http://spectrackulo.us/episode.php?episode_name=Top\\ Gear");
    }
    else if (socket->localPort() == 1203){
        system( "wget --quiet http://spectrackulo.us/episode.php?episode_name=Manchester\\ City\\ Vs\\ Chelsea");
    }
    exec();

}

void Thread::disconnected()
{
    qDebug() << socketDescriptor << "Disconnected: ";
    socket->deleteLater();
    system( "wget --quiet http://spectrackulo.us/episode.php?episode_name=  ");
    //exit(0);
}

FaceThread::FaceThread(int ID, QObject *parent) :
    Thread(parent)
{
    this->socketDescriptor = ID;
    bboxColor = QColor(0,255,0);
    bboxWidth = 2;
}

FaceThread::~FaceThread()
{
}

void FaceThread::readyRead()
{
    qDebug() << socketDescriptor << socket->bytesAvailable() << " bytes available";

    QByteArray response,imgByteArray;
    QDataStream out(&response,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    //if (socket->bytesAvailable() >= 8){

        int frameTime;
        QString searchString;
        //string label[5];
        string label;
        QString Url;
        in >> searchString;
        string line = searchString.toStdString();
        qDebug()<<searchString;
        istringstream liness;
        liness.clear();
        liness.str(line);
        qDebug() << "\n*************************************************************";
        liness >>frameTime;
        qDebug() << "Frame Time : " << frameTime;
        liness>>label;
        cout << label<<endl;
/*        for (int ii=0; ii<5; ii++){
            liness >>label[ii];
            qDebug() << "label" << ii << " is : " <<QString::fromStdString(label[ii]);
        }*/
        if(label=="Penny")
            system("wget --quiet http://spectrackulo.us/item.php?item=Kaley\\ Cuoco");
        else if(label=="Sheldon")
            system("wget --quiet http://spectrackulo.us/item.php?item=Jim\\ Parsons");
        else if(label=="Leonard")
            system("wget --quiet http://spectrackulo.us/item.php?item=Johnny\\ Galecki");
        else if(label=="Raj")
            system("wget --quiet http://spectrackulo.us/item.php?item=Kunal\\ Nayyar");
        else if(label=="Howard")
            system("wget --quiet http://spectrackulo.us/item.php?item=Simon\\ Helberg");
        else if(label=="Unknown")
            system("wget --quiet http://spectrackulo.us/item.php?item=  ");
        else
        {
            //sending R,G,B,width,Shape,episodeUrl,labelUrl
            QString tmpData=QString::number(this->bboxColor.red())+" "+QString::number(this->bboxColor.green())+" "+QString::number(this->bboxColor.blue())+" "+QString::number(this->bboxWidth)+" "+this->bboxShape+" ";
            out<<tmpData;
            qDebug()<<tmpData;
            socket->write(response);
            qDebug() << socketDescriptor << "Image compressed QByteArray Size : " << imgByteArray.size() << endl;
            qDebug() << socketDescriptor << "\nData written" << response.size() << "bytes";
            qDebug() << "*************************************************************";
        }
        qDebug()<<QString::fromStdString(Url.toStdString());
    //}
        socket->readAll();
}

ObjectThread::ObjectThread(int ID, QObject *parent) :
    Thread(parent)
{
    this->socketDescriptor = ID;
    bboxColor = QColor(255,0,0);
    bboxWidth = 1;
}

ObjectThread::~ObjectThread()
{
}

void ObjectThread::readyRead()
{
    qDebug() << socketDescriptor << socket->bytesAvailable() << " bytes available";
    QNetworkRequest request;
    QByteArray response,imgByteArray;
    QDataStream out(&response,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    //if (socket->bytesAvailable() >= 8){

        int frameTime;
        QString searchString;
        //string label[5];
        string label;
        QString Url;
        in >> searchString;
        string line = searchString.toStdString();
        istringstream liness;
        liness.clear();
        liness.str(line);
        qDebug() << "\n*************************************************************";
        liness >>frameTime;
        qDebug() << "Frame Time : " << frameTime;
        liness>>label;
        cout << label<<endl;
      /*  for (int ii=0; ii<5; ii++){
            liness >>label[ii];
            qDebug() << "label" << ii << " is : " <<QString::fromStdString(label[ii]);
        }*/
        if(label=="ferrari")
        {
            system("wget --quiet http://spectrackulo.us/item.php?item=Ferrari\\ California");
        }
        else if(label=="mclaren")
        {
            system("wget --quiet http://spectrackulo.us/item.php?item=Mclaren\\ MP4-12C");
        }
        else if(label=="unknown")
        {
            system("wget --quiet http://spectrackulo.us/item.php?item=  ");
        }
        else
        {
            //sending R,G,B,width,Shape,episodeUrl,labelUrl
            QString tmpData=QString::number(this->bboxColor.red())+" "+QString::number(this->bboxColor.green())+" "+QString::number(this->bboxColor.blue())+" "+QString::number(this->bboxWidth)+" "+this->bboxShape+" ";
            out<<tmpData;
            qDebug()<<tmpData;
            socket->write(response);
            qDebug() << socketDescriptor << "Image compressed QByteArray Size : " << imgByteArray.size() << endl;
            qDebug() << socketDescriptor << "\nData written" << response.size() << "bytes";
            qDebug() << "*************************************************************";
        }
        qDebug()<<QString::fromStdString(Url.toStdString());
    //}
        socket->readAll();
}

SportsThread::SportsThread(int ID, QObject *parent) :
    Thread(parent)
{
    this->socketDescriptor = ID;
}

SportsThread::~SportsThread()
{
}
void SportsThread::readyRead()
{
   /* qDebug() << socketDescriptor << socket->bytesAvailable() << " bytes available";

    QByteArray response,imgByteArray;
    QDataStream out(&response,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    if (socket->bytesAvailable() >= 8){

        qint64 frameTime;

        in >> frameTime;
        qDebug() << "\n*************************************************************";
        qDebug() << "Frame Time : " << frameTime;

        if (frameTime >= 26000 && frameTime <= 27000){
        // Read image;
        QImage img(QSize(640,480), QImage::Format_ARGB32_Premultiplied);  // Create the image with the exact size of the shrunk scene
        img.fill(Qt::transparent);
        QPen qpen(QColor(0,0,0),2);
        QPainter painter;
        painter.begin(&img);
        painter.setPen(qpen); // use 1 to draw, the pen with is 1 pixel solid line
        for (int ii=0; ii<numRect; ii++){
            in >> x >> y >> width >> height;
            qDebug() << "Rectangle " << ii << " at : " << x << y << width << height << label << searchString;
            painter.drawPixmap(QPixmap::fromImage());
            painter.drawText(QRect(x,y-10,width,8),label);
        }
        painter.end();

        // Serialize Image
        qDebug() << "\n*************************************************************";
        QBuffer buffer(&imgByteArray);
        img.save(&buffer, "PNG");
        qDebug() << "Uncompressed Image size : " << imgByteArray.size();
        imgByteArray = qCompress(imgByteArray);

        // Write Data
        out << (qint64)imgByteArray.size();
        out << imgByteArray;
        socket->write(response);
        qDebug() << socketDescriptor << "Image compressed QByteArray Size : " << imgByteArray.size() << endl;
        qDebug() << socketDescriptor << "\nData written" << response.size() << "bytes";
        qDebug() << "*************************************************************";
    }*/
}
