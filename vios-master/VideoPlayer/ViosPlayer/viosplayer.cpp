#include "viosplayer.h"
#include "ui_viosplayer.h"

ViosPlayer::ViosPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViosPlayer)
{
    ui->setupUi(this);
    videoPath.resize(4);
    QString path = "/home/rajeev/Dropbox/vios_team_use/";
    videoPath[0] = path + "demo_videos/tbbt-s04-e08_low.ts";
    videoPath[1] = path + "demo_videos/top_gear_mclaren_low.ts";
    videoPath[2] = path + "demo_videos/boondock_low.ts";
    videoPath[3] = path + "demo_videos/mancity_vs_chelsea_low.ts";

    videoChannel.resize(4);
    videoChannel[0] = "facerec";
    videoChannel[1] = "objdet";
    videoChannel[2] = "movie";
    videoChannel[3] = "sports";

    channelRect.resize(4);
    int xoff=0,yoff=0;
    channelRect[0] = QRect(84+xoff,105+yoff,360,270);
    channelRect[1] = QRect(915+xoff,102+yoff,360,270);
    channelRect[2] = QRect(82+xoff,407+yoff,360,270);
    channelRect[3] = QRect(916+xoff,405+yoff,360,270);

    channelSize.resize(4);
    channelSize[0]=QSize(854,480);
    channelSize[1]=QSize(1920,1080);
    channelSize[2]=QSize(576,240);
    channelSize[3]=QSize(1920,1080);

    XMLpath.resize(4);
    XMLpath[0]=path + "xml/facerec.xml";     //facerec
    XMLpath[1]=path + "xml/top_gear_new.xml";     //objDet
    XMLpath[2]=path + "xml/boondock.xml";               //movie
    XMLpath[3]=path + "xml/mancity_vs_chelsea.xml";     //sports

    curChannel = 0;

    wiiThread = new QThread(this);
    mainThread = QThread::currentThread();
    wii = new WiiWrapper();
    wii->moveToThread(wiiThread);
    wii->init();
    connect(wiiThread,SIGNAL(started()),wii,SLOT(poll()));
    connect(wiiThread, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(wii,SIGNAL(wiiSignal(const QString&)),this,SLOT(wiimoteHandler(const QString&)));
    wiiThread->start();

    // DataParser Initialization
    parserThread = new QThread(this);

    // Load HomeScreen Image
    homeBackground = path + "demo_videos/home_low.png";
    homeImage.load(homeBackground);
    player = new VideoPlayer(Phonon::VideoCategory, parent);
    connect(player,SIGNAL(finished()),this,SLOT(showHomeScreen()));
    player->play(QString(homeBackground));
    usleep(1000000);
    player->pause();
    player->showFullScreen();
    timer = new QTimer();
    timer->setInterval(10);
    connect(timer,SIGNAL(timeout()),this,SLOT(processOverlay()));

    overlayView = new QGraphicsView();
    overlayScene = new QGraphicsScene();

    overlayView->setFrameStyle( QFrame::NoFrame );
    overlayView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    overlayView->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    overlayView->setBackgroundRole( QPalette::Window );
    overlayView->setStyleSheet("background:transparent");
    overlayView->setAttribute(Qt::WA_TranslucentBackground);
    //overlayView->setSceneRect(QRect(0,0,homeImage.size().width(),homeImage.size().height()));
    //overlayView->setSceneRect(player->rect());
    overlayView->setBaseSize(player->size());
    overlayView->setScene(overlayScene);
    overlayView->showFullScreen();
    qDebug() << "player rect : " << player->rect();
    qDebug() << "player width: " << player->size();
    highlightChannel();

    showDetect=0;
    buttonOne = 0;
    curActive = 0;
    activeCount = 0;
    screen = "home";
    //display = new QStackedLayout();
    //display->addWidget(overlayView);
    //display->addWidget(player);
    //display->setStackingMode(QStackedLayout::StackAll);
    //setLayout(display);

    //display = new QStackedLayout();
    //display->addWidget(overlayView);
    //display->addWidget(player);
    player->stackUnder(overlayView);
    //overlayView->stackUnder(player);
    //display->setStackingMode(QStackedLayout::StackAll);
    sportsUrl = "?";
    prevsportsUrl="-";

    client=new CloudClient;
    hostAdd="192.168.1.2";
    //connect(client,SIGNAL(updateOverlay()),this,SLOT(modifyOverlay()));
}

void ViosPlayer::showHomeScreen(){
    player->play(homeBackground);

    if(hostPort!=-1)
    client->disconnectFromServer();

    timer->stop();
    overlayScene->clear();
    player->pause();
    usleep(300000);
    //overlayView->setSceneRect(player->rect());
    overlayView->setFixedSize(player->size());
    //overlayView->setSceneRect(QRect(0,0,homeImage.size().width(),homeImage.size().height()));
    //overlayView->setFixedSize(homeImage.size());
    highlightChannel();
    screen = "home";
}

void ViosPlayer::highlightChannel(){
    //qDebug() << "highlight : player size : " << player->size();
    overlayScene->clear();
    overlayScene->addRect(channelRect[curChannel],QPen(QColor(255,0,0),6));
}
// sports team1_name team2_name team1_goal team2_goal celeb_pattern
// movie ambience vibrator solenoid

void ViosPlayer::playChannel(){
    if (parserThread->isRunning()){
        qDebug() << "Quit thread safely";
        parser->terminateFlag = 1;
        //while(parserThread->isRunning());
        parserThread->quit();
    }
    parser = new DataParser();
    parserThread = new QThread(this);

    connect(parserThread,SIGNAL(started()),parser,SLOT(parseXML()));
    //connect(parserThread, SIGNAL(finished()), parser, SLOT(changeThread()));
    parser->dataQueue.clear();

    //parserThread->exit(0);
    parser->loadXML(XMLpath[curChannel]);
    //parser->moveToThread(parserThread);
    parserThread->start();
    frameData.clear();
    serverType(curChannel);

    if(hostPort!=-1)
    client->connectToServer(hostAdd,hostPort);

    player->play(QString(videoPath[curChannel]));
    timer->start();
    overlayScene->clear();
    qDebug() << "Player Rect : " << player->rect() << "Player Size : " << player->size();
    //overlayView->setSceneRect(player->rect());
    overlayView->setFixedSize(player->size());

    //curChannel++;
    if (curChannel==4)
        curChannel = 0;
    screen = "video";
}

void ViosPlayer::serverType(int channel)
{
    if(channel==0)
        hostPort=1200;
    else if(channel==1)
        hostPort=1201;

    else if(channel==3)
        hostPort=1202;
    else
        hostPort=-1;
}

void ViosPlayer::processOverlay(){
    if ((curChannel == 0 || curChannel==1) && buttonOne){
        //qDebug() << "Frame Data Size : " << frameData.size() << prevframeData.size();
        QMap<QString, QString>::const_iterator kk = prevframeData.constBegin();
        frameData.clear();
        while (kk != prevframeData.constEnd()) {
            frameData.insert(kk.key(),kk.value());
            ++kk;
        }
    }
    if (frameData.size()==0 && parser->dataQueue.size()!=0){
            frameData = parser->dataQueue.dequeue();
            QMap<QString, QString>::const_iterator kk = frameData.constBegin();
            while (kk != frameData.constEnd()) {
                prevframeData.insert(kk.key(),kk.value());
                ++kk;
            }
    }
    long long int curTime = player->currentTime(), overlayTime = frameData["time"].toLongLong();
    bool condn;
    if (curChannel==0 || curChannel==1){
        curTime -= 650;
        condn = overlayTime >= curTime && curTime >= overlayTime - 5;
    }
    else{
        condn = overlayTime<=curTime && curTime < overlayTime+10;
    }
      //qDebug() << "current time : " << curTime << "overlay time : " << overlayTime;


    if (condn){
        QMap<QString, QString>::const_iterator i = frameData.constBegin();
        sendData.clear();
        //sendData+=QString::number(player->width())+" "+QString::number(player->height())+" ";

        if(curChannel!=0 && curChannel!=1)
        while (i != frameData.constEnd()) {
            //qDebug() << i.key() << ": " << i.value() << endl;
            sendData+=i.value();
            sendData+=" ";
            ++i;
        }
        //qDebug() << "Player Size : " << player->size();
        if(curChannel==0 || curChannel==1)
        {
            sendData =frameData["time"] + " nil";
        //qDebug()<<sendData<<endl;
        }

        if (hostPort!=-1)
        client->sendToServer(sendData);
        if (curChannel!=3){
            sportsUrl = "wget --quiet http://spectrackulo.us/sports.php?clear=true";
            if (sportsUrl!=prevsportsUrl)
                system(sportsUrl.toStdString().c_str());
            prevsportsUrl = sportsUrl;
        }


        if(curChannel==3)
        {
            QString sdata="sudo echo sports "+ frameData["team1_name"] + " " + frameData["team2_name"] + " " + frameData["team1_goal"] + " " + frameData["team2_goal"] + " " + frameData["celeb_pattern"];
            sdata += " | tee /dev/ttyACM0";
            sportsUrl = "wget --quiet http://spectrackulo.us/sports.php?team1name=" + frameData["team1_name"] + "\\&team2name=" + frameData["team2_name"] + "\\&team1score=" + frameData["team1_goal"] + "\\&team2score=" + frameData["team2_goal"] + "\\&pattern=" + frameData["celeb_pattern"];
            if (sportsUrl!=prevsportsUrl)
                system(sportsUrl.toStdString().c_str());
            prevsportsUrl = sportsUrl;

            string sendata=sdata.toStdString();
            system(sendata.c_str());
        }
        else if(curChannel==2)
        {
            QString sdata="sudo echo action BoondockSaints "+ frameData["ambience"] + " " + frameData["vibrator"] + " " + frameData["solenoid"];
            sdata += " | tee /dev/ttyACM0";
            string sendata=sdata.toStdString();
            system(sendata.c_str());
        }
        overlayScene->clear();
        hratio = player->width()*1.0/channelSize[curChannel].width();
        vratio = player->height()*1.0/channelSize[curChannel].height();
        if ((curChannel == 0 || curChannel == 1) && showDetect){
            QPen pen;
            if (buttonOne)
                pen = QPen(QColor(255,0,0),client->lineWidth);
            else
                pen = QPen(client->col,client->lineWidth);
            activeCount =0;
            curActive = 0;
            if (frameData["o1_width"]!="0") activeCount++;
            if (frameData["o2_width"]!="0") activeCount++;
            if (frameData["o3_width"]!="0") activeCount++;
            if (frameData["o4_width"]!="0") activeCount++;
            if (frameData["o5_width"]!="0") activeCount++;
            if (buttonOne)
                overlayScene->addRect(QRect(frameData["o1_x"].toInt()*hratio,frameData["o1_y"].toInt()*vratio,frameData["o1_width"].toInt()*hratio,frameData["o1_height"].toInt()*vratio),QPen(QColor(0,255,0),3));
            else
                overlayScene->addRect(QRect(frameData["o1_x"].toInt()*hratio,frameData["o1_y"].toInt()*vratio,frameData["o1_width"].toInt()*hratio,frameData["o1_height"].toInt()*vratio),pen);
            overlayScene->addRect(QRect(frameData["o2_x"].toInt()*hratio,frameData["o2_y"].toInt()*vratio,frameData["o2_width"].toInt()*hratio,frameData["o2_height"].toInt()*vratio),pen);
            overlayScene->addRect(QRect(frameData["o3_x"].toInt()*hratio,frameData["o3_y"].toInt()*vratio,frameData["o3_width"].toInt()*hratio,frameData["o3_height"].toInt()*vratio),pen);
            overlayScene->addRect(QRect(frameData["o4_x"].toInt()*hratio,frameData["o4_y"].toInt()*vratio,frameData["o4_width"].toInt()*hratio,frameData["o4_height"].toInt()*vratio),pen);
            overlayScene->addRect(QRect(frameData["o5_x"].toInt()*hratio,frameData["o5_y"].toInt()*vratio,frameData["o5_width"].toInt()*hratio,frameData["o5_height"].toInt()*vratio),pen);
            QFont font = QFont("Times",8);
            QGraphicsTextItem *ttmp;
            if (frameData["o1_label"]!="Unknown"){
                ttmp = overlayScene->addText(frameData["o1_label"],font);
                ttmp->setPos(frameData["o1_x"].toInt()*hratio,frameData["o1_y"].toInt()*vratio-10);
                ttmp->setDefaultTextColor(client->col);
            }
            if (frameData["o2_label"]!="Unknown"){
                ttmp = overlayScene->addText(frameData["o2_label"],font);
                ttmp->setPos(frameData["o2_x"].toInt()*hratio,frameData["o2_y"].toInt()*vratio-10);
                ttmp->setDefaultTextColor(client->col);
            }
            if (frameData["o3_label"]!="Unknown"){
                ttmp = overlayScene->addText(frameData["o3_label"],font);
                ttmp->setPos(frameData["o3_x"].toInt()*hratio,frameData["o3_y"].toInt()*vratio-10);
                ttmp->setDefaultTextColor(client->col);
            }
            if (frameData["o4_label"]!="Unknown"){
                ttmp = overlayScene->addText(frameData["o4_label"],font);
                ttmp->setPos(frameData["o4_x"].toInt()*hratio,frameData["o4_y"].toInt()*vratio-10);
                ttmp->setDefaultTextColor(client->col);
            }
            if (frameData["o5_label"]!="Unknown"){
                ttmp = overlayScene->addText(frameData["o5_label"],font);
                ttmp->setPos(frameData["o5_x"].toInt()*hratio,frameData["o5_y"].toInt()*vratio-10);
                ttmp->setDefaultTextColor(client->col);
            }

        }
        //overlayScene->addPixmap(QPixmap::fromImage(client->overlayImage));
        overlayView->setScene(overlayScene);
        frameData.clear();
    }
    else if (overlayTime < curTime){
        while (overlayTime < curTime && parser->dataQueue.size()>0){
            frameData = parser->dataQueue.dequeue();
            overlayTime = frameData["time"].toInt();
        }
    }
}
void ViosPlayer::modifyOverlay(){
    overlayScene->clear();
    for (int ii=0;ii<5;ii++){
        if (ii==curActive)
            overlayScene->addRect(QRect(frameData["o"+QString::number(ii)+"_x"].toInt()*hratio,frameData["o"+QString::number(ii)+"_y"].toInt()*vratio,frameData["o"+QString::number(ii)+"_width"].toInt()*hratio,frameData["o"+QString::number(ii)+"_height"].toInt()*vratio),QPen(QColor(0,255,0),3));
        else
            overlayScene->addRect(QRect(frameData["o"+QString::number(ii)+"_x"].toInt()*hratio,frameData["o"+QString::number(ii)+"_y"].toInt()*vratio,frameData["o"+QString::number(ii)+"_width"].toInt()*hratio,frameData["o"+QString::number(ii)+"_height"].toInt()*vratio),QPen(QColor(255,0,0),2));
    }
}

void ViosPlayer::wiimoteHandler(const QString& key){
    qDebug() << "Wii Signal : " << key;
    if (screen == "home"){
        if (key == "RIGHT"){
            curChannel++;
            if (curChannel==4)
                curChannel = 0;
            highlightChannel();
        }
        else if (key == "LEFT"){
            curChannel--;
            if (curChannel==-1)
                curChannel = 3;
            highlightChannel();
        }
        else if (key == "DOWN"){
            curChannel += 2;
            if (curChannel>=4)
                curChannel -= 4;
            highlightChannel();
        }
        else if (key == "UP"){
            curChannel -= 2;
            if (curChannel<=-1)
                curChannel += 4;
            highlightChannel();
        }
        else if (key == "A"){
            qDebug()<<"Channel #"<<curChannel<< "Played";
            playChannel();
        }
    }
    else {
        if ((curChannel==0 || curChannel == 1) && buttonOne && key == "TWO"){
            if (curActive>0 && curActive<=5){
                //qDebug() << "Prev Frame Data : " << prevframeData.size() << "o"+QString::number(curActive)+"label";
                sendData =prevframeData["time"] + " " + prevframeData["o"+QString::number(curActive)+"_label"];
                qDebug()<<"Detetion Sent :" <<sendData;
                if (hostPort!=-1)
                client->sendToServer(sendData);
            }
        }
        if ((curChannel==0 || curChannel == 1) && buttonOne && key == "LEFT"){
            curActive--;
            if (curActive < 1) curActive = 1;
            modifyOverlay();
        }
        if ((curChannel==0 || curChannel == 1) && buttonOne && key == "RIGHT"){
            curActive++;
            if (curActive > activeCount) curActive = activeCount;
            modifyOverlay();
        }
        if (key == "HOME"){
            qDebug() << "showing HomeScreen";
            buttonOne = 0;
            showHomeScreen();

        }
        else if (key == "PLUS"){
            player->setVolume(player->volume()+0.05);
            qDebug() << "Volume Increased to :"<<player->volume();
        }
        else if (key == "MINUS"){
            player->setVolume(player->volume()-0.05);
            qDebug() << "Volume Decreased to :"<<player->volume();
        }
        else if ((curChannel==0 || curChannel == 1) && !buttonOne && key=="B"){
            if (showDetect)
                qDebug()<<"Detections Enabled";
            else{
                qDebug()<<"Detections Disabled";
                overlayScene->clear();
            }
            showDetect=!showDetect;
        }
        else if (key=="A")
        {
            if (buttonOne){
                timer->start();
            }
            buttonOne = 0;
            if(player->isPlaying()){
                qDebug()<<"Channel #"<<curChannel<< "Paused";
                player->pause();
            }
            else{
                qDebug()<<"Channel #"<<curChannel<< "Resumed";
                player->play();
            }
        }
        else if ((curChannel==0 || curChannel == 1) && !buttonOne && key=="ONE"){
            if(player->isPlaying()){
                player->pause();
                timer->stop();
            }
            showDetect = 1;
            buttonOne = 1;
            processOverlay();
        }
    }
}

ViosPlayer::~ViosPlayer()
{
    delete ui;
}
