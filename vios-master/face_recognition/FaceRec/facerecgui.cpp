#include "facerecgui.h"
#include "ui_facerecgui.h"

FacerecGui::FacerecGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FacerecGui)
{
    ui->setupUi(this);
}

FacerecGui::~FacerecGui()
{
    delete ui;
}
