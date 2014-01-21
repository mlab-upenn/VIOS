/**
 *  Tanvir Ahmed
 *  Project VIOS
 *
 *  Description: Takes in inputs from a Wii Remote and prints out what was pressed
 *  onto standard out.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <wiiuse.h>
//#include <libpcan.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_WIIMOTES    1

//handling method that handles different wii remote events
void handle_event(struct wiimote_t* wm)
{
    printf("\n\n--- EVENT [id %i] ---\n", wm->unid);

    /* if a button is pressed, report it */
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_A))		printf("A pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_B))		printf("B pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP))		printf("UP pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))	printf("DOWN pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT))	printf("LEFT pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))	printf("RIGHT pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS))	printf("MINUS pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS))	printf("PLUS pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE))		printf("ONE pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO))		printf("TWO pressed\n");
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME))	printf("HOME pressed\n");
}

//handling method that handles the case when a wii remote gets disconnected
void handle_disconnect(wiimote* wm) {
    printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
}


int main(int argc, const char *argv[])
{
    wiimote *wm;
    wiimote** wiimotes;
    int index;
    int found, connected;
    int wiimote_led_state;
    int exit = 0;

    wiimotes =  wiiuse_init(MAX_WIIMOTES);

    found = wiiuse_find(wiimotes, MAX_WIIMOTES, 5);
    if (!found) {
        printf ("No wiimotes found.\n");
        return 0;
    }

    connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
    if (connected)
        printf("Connected to %i wiimotes (of %i found).\n", connected, found);
    else {
        printf("Failed to connect to any wiimote.\n");
        return 0;
    }

    wm = wiimotes[0];
    wiiuse_status(wm);
    while(wm->event != WIIUSE_STATUS)
    {
        wiiuse_poll(wiimotes, MAX_WIIMOTES);
    }
    
    printf("Battery level: %f%%\n", wm->battery_level*100);

    while (1) {
        if (wiiuse_poll(wiimotes, MAX_WIIMOTES))  {
            /*
             *	This happens if something happened on any wiimote.
             *	So go through each one and check if anything happened.
             */
            for (index = 0; index < MAX_WIIMOTES; ++index) {
                switch (wiimotes[index]->event) {
                    
                    case WIIUSE_EVENT:
                        /* a generic event occured */
                        handle_event(wiimotes[index]);
                        break;
                    
                    case WIIUSE_UNEXPECTED_DISCONNECT:
                        /* the wiimote disconnected */
                        handle_disconnect(wiimotes[index]);
                        wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
                        return;
                    default:
                        break;
                }
            }
        }
    }
    wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
    return 0;

}
