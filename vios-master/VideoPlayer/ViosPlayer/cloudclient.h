#ifndef CLOUDCLIENT_H
#define CLOUDCLIENT_H

#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>
#include <QImage>
#include <QByteArray>
#include <QBuffer>
#include <QObject>
#include <QColor>
#include <sstream>

using namespace std;

class CloudClient : public QObject
{
    Q_OBJECT
public:
    explicit CloudClient(QObject * parent = 0);

signals:
    void error(QTcpSocket::SocketError socketError);

public:
    void connectToServer(QString host, qint16 port);
    void disconnectFromServer();
    void sendToServer(QString &data);
    void receiveFromServer(QImage & image);

signals:
    void receivedInfo();
    void updateOverlay();

private slots:
    void readyRead();
    void disconnected();

private:
    QDataStream in;
    QByteArray inData;
    qint64 packetByteCount;

public:
    QImage overlayImage;
    QColor col;
    int lineWidth;
    QString shape;
    QTcpSocket * socket;
};

#endif // NETZYNCLIENT_H
