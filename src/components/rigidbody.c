#include "rigidbody.h"
#include "physics/physic.h" // for aabb function
#include "xtypes.h"
#include "tiled/tiled_map.h"
#include "core/camera.h"
#include "core/logger.h"
#include "physics/physic_def.h"
#include "core/game_config.h"
#include "core/update_policy.h"
#include "components/entity.h"

static RigidBody bodyList[MAX_BODIES];
static u16 nextFree;  // Próximo slot livre
static u16 activeCount;  // Número de corpos ativos

void rigidbody_init(){
    // Inicializa a lista de livres
    for (u16 i = 0; i < MAX_BODIES - 1; i++) {
        bodyList[i].index = i + 1;  // Aponta para o próximo slot
    }
    bodyList[MAX_BODIES - 1].index = 0xFFFF;  // Marca o fim
    
    nextFree = 0;  // Primeiro slot livre
    activeCount = 0;
}

RigidBody* rigidbody_create(UpdatePolicy physicsPolicy, u8 layer, u8 mask, u16 tag){
    if (nextFree == 0xFFFF) {
        debug_log("Erro: Lista de corpos cheia!");
        return NULL;
    }
    
    // Pega o próximo slot livre
    u16 index = nextFree;
    nextFree = bodyList[index].index;
    
    // Limpa o corpo
    RigidBody* body = &bodyList[index];
    body->physicsPolicy = physicsPolicy;
    body->acceleration = FIX16(0);
    body->deceleration = FIX16(0);
    body->position.x = 0;
    body->position.y = 0;
    body->globalPosition.x = 0;
    body->globalPosition.y = 0;
    body->centerOffset.x = 0;
    body->centerOffset.y = 0;
    body->aabb.min.x = 0;
    body->aabb.min.y = 0;
    body->aabb.max.x = 0;
    body->aabb.max.y = 0;
    body->velocity.fixX = 0;
    body->velocity.x = 0;
    body->velocity.fixY = 0;
    body->delta.x = FIX16(0);
    body->delta.y = FIX16(0);
    body->layer = layer;
    body->mask = mask;
    body->tag = tag;
    body->owner = NULL;
    body->onCollision = NULL;
    body->onTrigger = NULL;
    body->physics = &defaultPhysicsParams;
    body->support = NULL;
    body->vState = VSTATE_AIRBORNE;
    body->mState = MSTATE_IDLE;
    body->aState = ASTATE_NONE;
    body->collidable = FALSE;
    body->active = FALSE;
    body->index = index;
    
    activeCount++;

    debug_log("Info: Corpo rigidbody criado com sucesso!");
    return body;
}

void rigidbody_destroy(RigidBody* body) {
    if (!body) return;
    
    u16 index = body->index;
    
    // Limpa o corpo
    body->acceleration = FIX16(0);
    body->deceleration = FIX16(0);
    body->position.x = 0;
    body->position.y = 0;
    body->globalPosition.x = 0;
    body->globalPosition.y = 0;
    body->centerOffset.x = 0;
    body->centerOffset.y = 0;
    body->aabb.min.x = 0;
    body->aabb.min.y = 0;
    body->aabb.max.x = 0;
    body->aabb.max.y = 0;
    body->velocity.fixX = 0;
    body->velocity.x = 0;
    body->velocity.fixY = 0;
    body->delta.x = FIX16(0);
    body->delta.y = FIX16(0);
    body->layer = 0;
    body->mask = 0;
    body->tag = 0;
    body->owner = NULL;
    body->onCollision = NULL;
    body->onTrigger = NULL;
    body->physics = NULL;
    body->support = NULL;
    body->vState = VSTATE_AIRBORNE;
    body->mState = MSTATE_IDLE;
    body->aState = ASTATE_NONE;
    body->collidable = TRUE;
    body->active = TRUE;
    
    // Adiciona à lista de livres
    body->index = nextFree;
    nextFree = index;
    
    activeCount--;
}

RigidBody* getRigidBody(u16 index){
    if (index >= MAX_BODIES) return NULL;
    return &bodyList[index];
}

/*
 */
 void rigidbody_update(RigidBody* body) {
    if (!body || !body->active) return;

    Vect2D_s16 previous = body->globalPosition;
    Entity* e = body->owner;
    // Aplica gravidade apenas se não tiver a flag FLAG_IGNORE_GRAVITY
    if(!(e->flags & FLAG_IGNORE_GRAVITY)){
        if(body->velocity.fixY <= body->physics->maxFallSpeed)
            body->velocity.fixY += body->physics->gravity;
        else
            body->velocity.fixY = body->physics->maxFallSpeed;
    }

    // Aplica movimento
    body->globalPosition.x += body->velocity.x;
    body->globalPosition.y += F16_toInt(body->velocity.fixY);
    
    if(body->mask & (1 << MASK_PLATFORM))
        return;
        
    // Atualiza delta
    body->delta.x = body->globalPosition.x - previous.x;
    body->delta.y = body->globalPosition.y - previous.y;
}