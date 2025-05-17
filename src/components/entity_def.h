#ifndef ENTITY_DEF_H
#define ENTITY_DEF_H

#include "sprite.h"
#include "rigidbody_def.h"
#include "core/anim_controller.h"
#include "core/update_policy.h"
#include "path_follower.h"
#include "entities/item_def.h"
#include "components/trigger_def.h"
#include "entities/item_def.h"
#include "dialogue.h"


typedef enum {
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_GENERIC,
    ENTITY_TYPE_NPC,
    ENTITY_TYPE_ENEMY,
    ENTITY_TYPE_PLATFORM,
    ENTITY_TYPE_ITEM,
    ENTITY_TYPE_TRIGGER
} EntityType;

typedef enum {
    ENTITY_EVENT_LAND,
    ENTITY_EVENT_JUMP,
    ENTITY_EVENT_HIT,
    ENTITY_EVENT_DIE
} EntityEventType;

typedef struct Entity {    
    u16 index;  // Índice no array interno    
    bool active;
    bool wasOnGround;    
    u16 flags; // Flags de controle

    EntityType tipo;    
    AnimController anim;
    RigidBody* body;
    UpdatePolicy logicPolicy;
    UpdatePolicy drawPolicy;
    DialogueState* dialogue;

    void (*onUpdate)(struct Entity*);
    void (*onDraw)(struct Entity*);
    void (*joyHandle)(u16 joy, u16 changed, u16 state);
    void (*onDestroy)(struct Entity*);
    void (*onEndPath)(struct Entity*); // substitui onEnd do PathAgent
    void (*onInteract)(struct Entity*);
    void (*onEvent)(struct Entity* self, EntityEventType type);

    void* pData;
}Entity;

#endif