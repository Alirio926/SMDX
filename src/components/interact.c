#include "interact.h"
#include "entity_def.h"
#include "core/camera.h"
#include "physics/physic.h"
#include "physics/physic_def.h"
#include "components/entity.h"
#include "core/logger.h"
#include "entities/npc_simple.h"

extern Entity entityList[MAX_ENTITIES];

void tryInteract(Entity* player) {
    const Vect2D_f16 playerCenter = getBodyCenter(player->body);
    const AABB* cam = camera_getScreenBounds();

    for (u16 i = 0; i < MAX_ENTITIES; i++) {
        Entity* e = getEntity(i);
        if (!e->active || !e->onInteract) continue;
        if (!(e->flags & FLAG_INTERACTABLE)) continue;
        
        if (e->body) {         
            if (!(player->body->mask & (1 << e->body->layer))) continue;   
            if (!aabb_intersect(&e->body->aabb, cam)) continue;
            if (!isNear(playerCenter, getBodyCenter(e->body), INTERACT_RADIUS)) continue;
            if (aabb_intersect(&player->body->aabb, &e->body->aabb)) {
                e->onInteract(e);
                return;
            }
        }
        
        ItemDef* itemDef = NULL;
        NpcSimpleDef* npcDef = NULL;
        Box* box = NULL;

        switch(e->tipo){
            case ENTITY_TYPE_ITEM:
                itemDef = (ItemDef*)e->pData; 
                box = &itemDef->hitbox; break;
            case ENTITY_TYPE_NPC:
                npcDef = (NpcSimpleDef*)e->pData;
                box = &npcDef->hitbox; break;
            default:
                box = NULL;
        }

        if (box) {  
            if (!aabb_intersectBox(box, cam)) continue;   
            if (!isNear(playerCenter, getBoxCenter(box), INTERACT_RADIUS)) continue;
            AABB globalAABB;
            rigidbody_getGlobalAABB(player->body, &globalAABB);
            debug_log("Tipo: %d", e->tipo);
            if (aabb_intersectBox(box, &globalAABB)) {
                e->onInteract(e);
                return;
            }
        }
    }
}

bool isNear(Vect2D_f16 a, Vect2D_f16 b, fix16 range) {
    fix16 dx = F16_abs(a.x - b.x);
    fix16 dy = F16_abs(a.y - b.y);
    return (dx + dy) < range; // Manhattan distance
}

Vect2D_f16 getBodyCenter(RigidBody* body) {
    fix16 px = body->globalPosition.x;
    fix16 py = body->globalPosition.y;

    s16 width  = body->aabb.max.x - body->aabb.min.x;
    s16 height = body->aabb.max.y - body->aabb.min.y;

    return (Vect2D_f16){
        px + F16_toInt(width >> 1),
        py + F16_toInt(height >> 1)
    };
}

Vect2D_f16 getBoxCenter(Box* box) {
    fix16 px = box->x;
    fix16 py = box->y;
    return (Vect2D_f16){
        px + F16_toInt(box->w >> 1),
        py + F16_toInt(box->h >> 1)
    };
}
