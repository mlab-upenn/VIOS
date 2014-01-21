#include "wiiwrapper.h"

WiiWrapper::WiiWrapper()
{
    MAX_WIIMOTES = 1;
    exit = 0;
}

WiiWrapper::~WiiWrapper(){
    wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
}

void WiiWrapper::init(){
    wiimotes =  wiiuse_init(MAX_WIIMOTES);
    found = wiiuse_find(wiimotes, MAX_WIIMOTES, 5);
    if (!found)
        qDebug() << "No wiimotes found.";

    connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
    if (connected)
        qDebug() << "Connected to " << connected << "wiimotes (of " << found << " found)";
    else
        qDebug() << "Failed to connect to any wiimote.";

    wm = wiimotes[0];
    wiiuse_status(wm);
    while(wm->event != WIIUSE_STATUS){
        wiiuse_poll(wiimotes, MAX_WIIMOTES);
    }

    qDebug() << "Battery level: " << wm->battery_level*100 << "%%";
}

void WiiWrapper::poll(){
    while (1){
        if (wiiuse_poll(wiimotes, MAX_WIIMOTES)){
            // This happens if something happened on any wiimote.
            // So go through each one and check if anything happened.
            for (index = 0; index < MAX_WIIMOTES; ++index) {
                switch (wiimotes[index]->event) {
                    case WIIUSE_EVENT:
                        // a generic event occured
                        handle_event(wiimotes[index]);
                        break;
                    case WIIUSE_UNEXPECTED_DISCONNECT:
                         // the wiimote disconnected
                         handle_disconnect(wiimotes[index]);
                         wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
                         return;
                     default:
                         break;
                }
            }
        }
    }
}

void WiiWrapper::handle_event(struct wiimote_t* wm)
{
    printf("\n\n--- EVENT [id %i] ---\n", wm->unid);

    /* if a button is pressed, report it */
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_A)){
        qDebug() << "A pressed";
        emit wiiSignal("A");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_B)){
        qDebug() << "B pressed";
        emit wiiSignal("B");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP)){
        qDebug() << "UP pressed";
        emit wiiSignal("UP");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN)){
        qDebug() << "DOWN pressed";
        emit wiiSignal("DOWN");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT)){
        qDebug() << "LEFT pressed";
        emit wiiSignal("LEFT");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT)){
        qDebug() << "RIGHT pressed";
        emit wiiSignal("RIGHT");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS)){
        qDebug() << "PLUS pressed";
        emit wiiSignal("PLUS");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS)){
        qDebug() << "MINUS pressed";
        emit wiiSignal("MINUS");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME)){
        qDebug() << "HOME pressed";
        emit wiiSignal("HOME");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE)){
        qDebug() << "ONE pressed";
        emit wiiSignal("ONE");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO)){
        qDebug() << "TWO pressed";
        emit wiiSignal("TWO");
    }
}

//handling method that handles the case when a wii remote gets disconnected
void WiiWrapper::handle_disconnect(wiimote* wm) {
    qDebug() << "\n\n--- DISCONNECTED [wiimote id " << wm->unid << "] ---";
}

