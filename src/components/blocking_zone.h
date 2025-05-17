#ifndef BLOCKING_ZONE_H
#define BLOCKING_ZONE_H

#include <genesis.h>
#include "xtypes.h"
#include "sprite.h"
#include "components/rigidbody_def.h"
#include "core/game_config.h"
#include "components/trigger_def.h"

typedef struct {
    bool active;
    Box hitbox;
    Sprite* sprite;
    bool visible;
} BlockingZone;

void blocking_clearAll();
BlockingZone* blocking_create(s16 x, s16 y, s16 w, s16 h);
void blocking_enable(BlockingZone* zone);
void blocking_disable(BlockingZone* zone);
void physics_checkBlockingZones(RigidBody* body);
void blocking_setVisible(BlockingZone* zone, bool visible) ;

#endif
