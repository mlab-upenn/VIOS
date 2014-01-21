#include "facerecgui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FacerecGui w;
    w.show();
    
    return a.exec();
}
