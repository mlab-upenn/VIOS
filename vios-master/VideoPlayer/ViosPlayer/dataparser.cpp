#include "dataparser.h"

DataParser::DataParser()
{
}
void DataParser::loadXML(QString filename){
    xmlFile = new QFile(filename);
    qDebug()<<filename;
    // If we can't open it, let's show an error message.
    if (!xmlFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot Open XML file";
        //exit(1);
    }
    // QXmlStreamReader takes any QIODevice.
    xml.setDevice(xmlFile);
    terminateFlag = 0;
}

void DataParser::parseXML() {
    // We'll parse the XML until we reach end of it.
    while(!xml.atEnd() && !xml.hasError()) {
        if (terminateFlag == 1)
            break;
        // Read next element.
        QXmlStreamReader::TokenType token = xml.readNext();
        // If token is just StartDocument, we'll go to next.
        if(token == QXmlStreamReader::StartDocument)
            continue;
        // If token is StartElement, we'll see if we can read it.
        if(token == QXmlStreamReader::StartElement) {
            // If it's named persons, we'll go to the next.
            if(xml.name() == "data")
                continue;
            else if (xml.name() == "sports_detection_data"){
                dataType = "sports";
                continue;
            }
            else if (xml.name() == "movie_detection_data"){
                dataType = "movie";
                continue;
            }
            else if (xml.name() == "face_detection_data"){
                dataType = "facerec";
                continue;
            }
            else if (xml.name() == "object_detection_data"){
                dataType = "objdet";
                continue;
            }
            // If it's named row, we'll dig the information from there.
            if(xml.name() == "row")
                    dataQueue.enqueue(this->parseData(xml));
        }
    }
    // Error handling.
    if(xml.hasError())
        qDebug() << "XML Parsing Error : " << xml.errorString();
    // Removes any device() or data from the reader and resets its internal state to the initial state.
    xml.clear();
}

QMap<QString, QString> DataParser::parseData(QXmlStreamReader& xml) {
    QMap<QString, QString> rowData;
    QString elementName;
    // Let's check that we're really getting a row.
    if(xml.tokenType() != QXmlStreamReader::StartElement && xml.name() != "person")
        return rowData;
    // Let's get the attributes for row
    QXmlStreamAttributes attributes = xml.attributes();
    // Let's check that row has time attribute.
    if(attributes.hasAttribute("time"))
        rowData["time"] = attributes.value("time").toString();
    //qDebug() << "time : " << rowData["time"];
    // Next element...
    xml.readNext();
    // We're going to loop over the things because the order might change.
    // We'll continue the loop until we hit an EndElement named row
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "row")) {
        if(xml.tokenType() == QXmlStreamReader::StartElement) {
            elementName = xml.name().toString();
            xml.readNext();
            rowData.insert(elementName, xml.text().toString());
            //qDebug()<<elementName<<" : " << xml.text().toString();
        }
        xml.readNext();
    }
    return rowData;
}
