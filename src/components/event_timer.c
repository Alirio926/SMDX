// event_timer.c — Sistema simples de eventos temporizados para SGDK
// -----------------------------------------------------------------------------
#include <genesis.h>
#include "event_timer.h"

#define MAX_TIMERS 8

typedef struct {
    u16 delay;
    u16 elapsed;
    EventCallback callback;
    bool active;
} TimerEvent;

static TimerEvent timers[MAX_TIMERS];

void eventTimer_resetAll() {
    for (u16 i = 0; i < MAX_TIMERS; i++)
        timers[i].active = FALSE;
}

bool eventTimer_schedule(u16 delayFrames, EventCallback callback) {
    for (u16 i = 0; i < MAX_TIMERS; i++) {
        if (!timers[i].active) {
            timers[i].delay = delayFrames;
            timers[i].elapsed = 0;
            timers[i].callback = callback;
            timers[i].active = TRUE;
            return TRUE;
        }
    }
    return FALSE; // sem espaço
}

void eventTimer_updateAll() {
    for (u16 i = 0; i < MAX_TIMERS; i++) {
        if (timers[i].active) {
            timers[i].elapsed++;
            if (timers[i].elapsed >= timers[i].delay) {
                timers[i].active = FALSE;
                if (timers[i].callback) timers[i].callback();
            }
        }
    }
}
