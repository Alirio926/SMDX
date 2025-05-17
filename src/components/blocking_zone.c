#include "blocking_zone.h"
#include "physics/physic.h"

static BlockingZone blockingList[MAX_BLOCKING_ZONES];
static u16 blockingCount = 0;

void blocking_clearAll() {
    blockingCount = 0;
    for(u16 i = 0; i < MAX_BLOCKING_ZONES; i++) {
        blockingList[i].active = FALSE;
    }
}

BlockingZone* blocking_create(s16 x, s16 y, s16 w, s16 h) {
    if (blockingCount >= MAX_BLOCKING_ZONES) return NULL;

    BlockingZone* b = &blockingList[blockingCount++];
    b->active = TRUE;
    b->hitbox = (Box){x, y, w, h};
    b->visible = TRUE;
    return b;
}

void blocking_enable(BlockingZone* zone) {
    if (zone) zone->active = TRUE;
}

void blocking_disable(BlockingZone* zone) {
    if (zone) zone->active = FALSE;
}

void physics_checkBlockingZones(RigidBody* body) {
    for (u16 i = 0; i < blockingCount; i++) {
        BlockingZone* b = &blockingList[i];
        if (!b->active)continue;

        AABB globalAABB;
        rigidbody_getGlobalAABB(body, &globalAABB);
        AABB blockerAABB = {
            .min.x = b->hitbox.x,
            .min.y = b->hitbox.y,
            .max.x = b->hitbox.x + b->hitbox.w,
            .max.y = b->hitbox.y + b->hitbox.h
        };
        if (aabb_intersect(&blockerAABB, &globalAABB)) {
            body->delta.x = 0;
            body->velocity.x = 0;
            body->velocity.fixX = 0;
            // Se estiver descendo, empurra para cima da barreira
            if (body->delta.y > 0) {
                // Colisão por baixo → empurra para cima
                body->globalPosition.y = blockerAABB.min.y - (body->aabb.max.y - body->aabb.min.y);
                body->delta.y = 0;
                body->velocity.fixY = 0;
                body->vState = VSTATE_GROUNDED;
            }
            else if (body->delta.y < 0) {
                // Colisão por cima → opcional, impede subir
                body->globalPosition.y = blockerAABB.max.y;
                body->delta.y = 0;
                body->velocity.fixY = 0;
            }
        }
    }
}

void blocking_setVisible(BlockingZone* zone, bool visible) {
    if(zone->visible != visible) {  
        zone->visible = visible;
        SPR_setVisibility(zone->sprite, visible ? VISIBLE : HIDDEN);
    }
}

