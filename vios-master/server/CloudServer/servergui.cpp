#include "servergui.h"
#include "ui_servergui.h"

ServerGui::ServerGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerGui)
{
    ui->setupUi(this);

    faceServer.startServer(1200);
    objectServer.startServer(1201);
    sportsServer.startServer(1202);

    configureUi();
}

void ServerGui::configureUi()
{
    // Face Server Default Properties
    ui->radioButtonFaceShapeRect->setChecked(true);
    ui->spinBoxFaceLineWidth->setValue(1);
    ui->radioButtonFaceColorBlue->setChecked(true);

    // Object Server Default Properties
    ui->radioButtonObjectShapeRect->setChecked(true);
    ui->spinBoxObjectLineWidth->setValue(1);
    ui->radioButtonObjectColorRed->setChecked(true);

    // Sports Server Default Properties
    sportsTeam1 = "Manchester United";
    sportsTeam2 = "Chelsea";

    ui->lineEditSportsTeam1->setText(sportsTeam1);
    ui->lineEditSportsTeam2->setText(sportsTeam2);

    sportsLogo1 = new QGraphicsScene();
    sportsLogo2 = new QGraphicsScene();
    ui->graphicsViewSportsLogo1->setScene(sportsLogo1);
    ui->graphicsViewSportsLogo2->setScene(sportsLogo2);

    if (sportsLogoImage1.load("../../images/" + sportsTeam1 + ".png"))
        sportsLogo1->addPixmap(QPixmap::fromImage(sportsLogoImage1.scaled(QSize(140,150),Qt::IgnoreAspectRatio)));
    if(sportsLogoImage2.load("../../images/" + sportsTeam2 + ".png"))
        sportsLogo2->addPixmap(QPixmap::fromImage(sportsLogoImage2.scaled(QSize(140,150),Qt::IgnoreAspectRatio)));

    // Face Server
    connect(ui->radioButtonFaceShapeRect, SIGNAL(clicked()), this,SLOT(faceShapeUpdate()));
    connect(ui->radioButtonFaceShapeCircle, SIGNAL(clicked()), this,SLOT(faceShapeUpdate()));

    connect(ui->radioButtonFaceColorBlack, SIGNAL(clicked()), this,SLOT(faceColorUpdate()));
    connect(ui->radioButtonFaceColorBlue, SIGNAL(clicked()), this,SLOT(faceColorUpdate()));
    connect(ui->radioButtonFaceColorGreen, SIGNAL(clicked()), this,SLOT(faceColorUpdate()));
    connect(ui->radioButtonFaceColorRed, SIGNAL(clicked()), this,SLOT(faceColorUpdate()));
    connect(ui->radioButtonFaceColorWhite, SIGNAL(clicked()), this,SLOT(faceColorUpdate()));
    connect(ui->spinBoxFaceLineWidth,SIGNAL(valueChanged(int)),this,SLOT(faceWidthUpdate(int)));

    // Object Server
    connect(ui->radioButtonObjectShapeRect, SIGNAL(clicked()), this,SLOT(objectShapeUpdate()));
    connect(ui->radioButtonObjectShapeCircle, SIGNAL(clicked()), this,SLOT(objectShapeUpdate()));

    connect(ui->radioButtonObjectColorBlack, SIGNAL(clicked()), this,SLOT(objectColorUpdate()));
    connect(ui->radioButtonObjectColorBlue, SIGNAL(clicked()), this,SLOT(objectColorUpdate()));
    connect(ui->radioButtonObjectColorGreen, SIGNAL(clicked()), this,SLOT(objectColorUpdate()));
    connect(ui->radioButtonObjectColorRed, SIGNAL(clicked()), this,SLOT(objectColorUpdate()));
    connect(ui->radioButtonObjectColorWhite, SIGNAL(clicked()), this,SLOT(objectColorUpdate()));
    connect(ui->spinBoxObjectLineWidth,SIGNAL(valueChanged(int)),this,SLOT(objectWidthUpdate(int)));

    ui->textBrowserSportsStatus->clear();
    ui->textBrowserSportsStatus->setText("Listening at port : 1202");

    ui->textBrowserSportsInfo->setText("12 Connecting\n12 Spawning Sports thread\n12 Client Connected\n12 Data Written\n");

    ui->textBrowserFaceStatus->setText("Listening at port : 1200");

    ui->textBrowserFaceInfo->setText("7 Connecting\n7 Spawning Face thread\n7 Client Connected\n7 Data Read\n\n7 Compressed Image Bytes : 1104\n7 Data Written\n");

    ui->textBrowserObjectStatus->setText("Listening at port : 1201");

    ui->textBrowserObjectInfo->setText("23 Connecting\n23 Spawning Object thread\n23 Client Connected\n23 Data Read\n23 Compressed Image Bytes : 1002\n23 Data Written\n");
}

void ServerGui::faceShapeUpdate()
{
    if(this->faceServer.threadStarted)
    {
    if(ui->radioButtonFaceShapeRect->isChecked())
        this->faceServer.faceThread->bboxShape="Rectangle";
    else
        this->faceServer.faceThread->bboxShape="circle";
    }
}

void ServerGui::faceColorUpdate()
{
    if(this->faceServer.threadStarted)
    {
    if(ui->radioButtonFaceColorBlack->isChecked())
        this->faceServer.faceThread->bboxColor = QColor(0,0,0);
    else if(ui->radioButtonFaceColorBlue->isChecked())
        this->faceServer.faceThread->bboxColor=QColor(0,0,255);
    else if(ui->radioButtonFaceColorGreen->isChecked())
        this->faceServer.faceThread->bboxColor=QColor(0,255,0);
    else if(ui->radioButtonFaceColorRed->isChecked())
        this->faceServer.faceThread->bboxColor=QColor(255,0,0);
    else if(ui->radioButtonFaceColorWhite->isChecked())
        this->faceServer.faceThread->bboxColor=QColor(255,255,255);
    }
}
void ServerGui::faceWidthUpdate(int x)
{
     if(this->faceServer.threadStarted)
     this->faceServer.faceThread->bboxWidth=x;
     qDebug()<<"X: "<<x;
}

void ServerGui::objectShapeUpdate()
{
    if(this->objectServer.threadStarted)
    {
    if(ui->radioButtonObjectShapeRect->isChecked())
        this->objectServer.objThread->bboxShape="Rectangle";
    else
        this->objectServer.objThread->bboxShape="circle";
    }
}
void ServerGui::objectColorUpdate()
{
    if(objectServer.threadStarted)
    {
    if(ui->radioButtonObjectColorBlack->isChecked())
        this->objectServer.objThread->bboxColor=QColor(0,0,0);
    else if(ui->radioButtonObjectColorBlue->isChecked())
        this->objectServer.objThread->bboxColor=QColor(0,0,255);
    else if(ui->radioButtonObjectColorGreen->isChecked())
        this->objectServer.objThread->bboxColor=QColor(0,255,0);
    else if(ui->radioButtonObjectColorRed->isChecked())
        this->objectServer.objThread->bboxColor=QColor(255,0,0);
    else if(ui->radioButtonObjectColorWhite->isChecked())
        this->objectServer.objThread->bboxColor=QColor(255,255,255);
    }
}

void ServerGui::objectWidthUpdate(int x)
{
    if(this->objectServer.threadStarted)
    this->objectServer.objThread->bboxWidth=x;
    qDebug()<<"x: "<<x;
}

ServerGui::~ServerGui()
{
    delete ui;
}
