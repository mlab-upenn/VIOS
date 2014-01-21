#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QString>
#include <QMap>
#include <QQueue>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QObject>

class DataParser : public QObject
{
    Q_OBJECT
public:
    QFile *xmlFile;
    QXmlStreamReader xml;
    QString dataType;
    QQueue< QMap < QString, QString > > dataQueue;
    bool terminateFlag;

    DataParser();
    void loadXML(QString filename);
    QMap < QString, QString> parseData(QXmlStreamReader& xml);
public slots:
    void parseXML();

};

#endif // DATAPARSER_H
