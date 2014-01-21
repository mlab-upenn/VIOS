#ifndef FACERECGUI_H
#define FACERECGUI_H

#include <QMainWindow>

namespace Ui {
class FacerecGui;
}

class FacerecGui : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit FacerecGui(QWidget *parent = 0);
    ~FacerecGui();
    
private:
    Ui::FacerecGui *ui;
};

#endif // FACERECGUI_H
