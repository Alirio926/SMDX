#include "object_factory.h"
#include "core/anim_controller.h"   // Define defaultAnimSet
#include "rigidbody.h"
#include "tiled/fase1_obj.h"
#include "physics/physic_def.h"
#include "sprite.h"     
#include "components/object_factory.h"
#include "components/entity.h"
#include "components/blocking_zone.h"
#include "types.h"
#include "trigger.h"
#include "core/camera.h"

Entity* spawn_entity_platform(u16 index, DistanceType distanceType) {
    const PathAgentDef* def = &platforms[index].agentDef; // Dados do parser

    Entity* entity = entity_create(
        UPDATE_ALWAYS,
        UPDATE_VISIBLE_ONLY
    );

    RigidBody* body = rigidbody_create(
        UPDATE_ALWAYS,
        LAYER_PLATFORM,
        MASK_PLATFORM,
        TAG_PLATFORM_MOVING
    );

    body->active = TRUE;
    body->collidable = TRUE; 
    body->owner = entity;
    body->globalPosition.x = def->position.x;
    body->globalPosition.y = def->position.y + 8;
    body->aabb = newAABB(0, def->w, 0, def->h);
    body->centerOffset = newVector2D_u16((body->aabb.min.x + body->aabb.max.x) / 2,
                                        (body->aabb.min.y + body->aabb.max.y) / 2);

    AnimController anim = {.sprite = NULL, .visible = true, .animSet = &defaultAnimSet};

    // --- entity ---    
    entity->active = true;
    entity->tipo = ENTITY_TYPE_PLATFORM;   
    entity->flags = FLAG_IGNORE_GRAVITY | FLAG_CAN_RIDE;

    // --- body ---
    entity->body = body;

    // --- SPRITE ---
    entity->anim.animSet = &defaultAnimSet;
    entity->anim.sprite = SPR_addSprite(&spr_platform, def->position.x, def->position.y, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    entity->anim.visible = true;
    PAL_setPalette(PAL2, spr_platform.palette->data, DMA);
    // --- Functions ---
    entity->onUpdate = follower_update;

    // --- PathFollower ---
    entity->pData = path_follower_create(def, distanceType);
        
    // --- animcontroller ---
    animcontroller_init(&entity->anim, entity->anim.sprite, anim.animSet);

    return entity;
}

Entity* create_trigger_with_blocking_zone(Vect2D_u16 triggerPosition, Vect2D_u16 blockingPosition, 
            const SpriteDefinition* triggerSprite, const SpriteDefinition* blockingSprite, 
            TriggerZoneAction zoneAction, TriggerType triggerType, AnimStateSet* animStateSet, 
            TriggerCallback onEnter, TriggerCallback onExit) {

    BlockingZone* zone = blocking_create(blockingPosition.x, blockingPosition.y, 
                                             blockingSprite->w, blockingSprite->h);
    
    zone->sprite = SPR_addSprite(blockingSprite, blockingPosition.x, blockingPosition.y, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));

    return entity_spawnTrigger(0, triggerPosition, triggerType, zoneAction, onEnter, onExit, triggerSprite, zone);
}


