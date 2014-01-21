#ifndef WIIWRAPPER_H
#define WIIWRAPPER_H

#include <wiiuse.h>
#include <QDebug>
#include <QObject>

class WiiWrapper : public QObject
{
    Q_OBJECT
public:
    wiimote *wm;
    wiimote** wiimotes;
    int index, found, connected, wiimote_led_state, exit, MAX_WIIMOTES;

    WiiWrapper();
    ~WiiWrapper();
    void init();
    void handle_event(struct wiimote_t* wm);
    void handle_disconnect(wiimote* wm);
public slots:
    void poll();
signals:
    void wiiSignal(const QString&);
};

#endif // WIIWRAPPER_H
