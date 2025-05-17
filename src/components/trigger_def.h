#ifndef _TRIGGER_DEF_H_
#define _TRIGGER_DEF_H_

#include "xtypes.h"
#include "sprite.h"
#include "components/blocking_zone.h"

typedef enum {
    TRIGGER_TYPE_ONCE = 0,
    TRIGGER_TYPE_REPEAT,
    TRIGGER_TYPE_ENTER_EXIT
} TriggerType;

typedef enum {
    TRIGGER_ZONE_NONE = 0,
    TRIGGER_ZONE_ENABLE,
    TRIGGER_ZONE_DISABLE,
    TRIGGER_ZONE_TOGGLE
} TriggerZoneAction;

typedef void (*TriggerCallback)(void* context);

typedef struct {
    Box hitbox;
    TriggerType type;
    bool active;
    bool triggered;
    TriggerCallback onEnter;
    TriggerCallback onExit;
    void* context;
    TriggerZoneAction zoneAction;
} TriggerDef;

#endif