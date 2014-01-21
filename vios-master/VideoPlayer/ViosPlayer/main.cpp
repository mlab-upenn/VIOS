#include "viosplayer.h"
#include <QApplication>
#include "dataparser.h"
#include <QDebug>
#include <QMap>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ViosPlayer w;
    //w.show();

    return a.exec();
}
