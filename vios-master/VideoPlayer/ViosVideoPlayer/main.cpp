#include "playergui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PlayerGui w;
    //w.showFullScreen();
    
    return a.exec();
}
