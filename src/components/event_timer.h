// event_timer.h — Interface de eventos temporizados
// -----------------------------------------------------------------------------
#ifndef EVENT_TIMER_H
#define EVENT_TIMER_H

#include <genesis.h>

typedef void (*EventCallback)(void);

void eventTimer_resetAll(void);
bool eventTimer_schedule(u16 delayFrames, EventCallback callback);
void eventTimer_updateAll(void);

#endif // EVENT_TIMER_H
