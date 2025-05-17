#ifndef _OBJECT_FACTORY_H_
#define _OBJECT_FACTORY_H_

#include <genesis.h>
#include "components/entity_def.h"
#include "components/path_def.h"

Entity* spawn_entity_platform(u16 index, DistanceType distanceType) ;

Entity* create_trigger_with_blocking_zone(Vect2D_u16 triggerPosition, Vect2D_u16 blockingPosition, 
            const SpriteDefinition* triggerSprite, const SpriteDefinition* blockingSprite, 
            TriggerZoneAction zoneAction, TriggerType triggerType, AnimStateSet* animStateSet, 
            TriggerCallback onEnter, TriggerCallback onExit);
            
#endif
