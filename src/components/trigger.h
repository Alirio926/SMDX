#ifndef _TRIGGER_H_
#define _TRIGGER_H_

#include "xtypes.h"
#include "physics/physic_def.h"
#include "components/entity_def.h"
#include "components/trigger_def.h"
#include "components/blocking_zone.h"

Entity* entity_spawnTrigger(u16 index, Vect2D_u16 position, TriggerType triggerType, TriggerZoneAction zoneAction, TriggerCallback onEnter, TriggerCallback onExit, SpriteDefinition* spriteDef, BlockingZone* zone);
void trigger_setTarget(Entity* player);
void trigger_applyZoneAction_wrapper(void* ctx);

#endif
