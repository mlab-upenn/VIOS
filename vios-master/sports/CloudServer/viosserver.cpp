#include "viosserver.h"
#include "ui_server.h"

ServerGui::ServerGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerGui)
{
    ui->setupUi(this);
    //faceServer.startServer(1200);
    //objectServer.startServer(1200);
    //sportsServer.startServer(1200);
}

ServerGui::~ServerGui()
{
    delete ui;
}
