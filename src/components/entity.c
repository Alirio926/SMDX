#include <genesis.h>
#include "entity.h"
#include "core/game_config.h"
#include "core/camera.h"
#include "core/logger.h"
#include "entities/npc_simple.h"

void entity_setVisible(Entity* e, bool visible) ;
void entity_getAABB(Entity* e, AABB* out);
Vect2D_s16 entity_getGlobalPosition(Entity* e);

static Entity entityList[MAX_ENTITIES];
static u16 nextFree;  // Próximo slot livre
static u16 activeCount;  // Número de entidades ativas
extern DialogueState activeDialogue;

void init_entity() {
    // Inicializa a lista de livres
    for (u16 i = 0; i < MAX_ENTITIES - 1; i++) {
        entityList[i].index = i + 1;  // Aponta para o próximo slot
    }
    entityList[MAX_ENTITIES - 1].index = 0xFFFF;  // Marca o fim
    
    nextFree = 0;  // Primeiro slot livre
    activeCount = 0;
}

Entity* entity_create(UpdatePolicy logicPolicy, UpdatePolicy drawPolicy) {
    if (nextFree == 0xFFFF) {
        debug_log("Erro: Lista de entidades cheia!");
        return NULL;
    }
    
    // Pega o próximo slot livre
    u16 index = nextFree;
    nextFree = entityList[index].index;
    
    // Limpa a entidade
    Entity* entity = &entityList[index];
    entity->dialogue = &activeDialogue;
    entity->body = NULL;
    entity->joyHandle = NULL;
    entity->onUpdate = NULL;
    entity->onDraw = NULL;
    entity->pData = NULL;
    entity->index = index;
    entity->anim.sprite = NULL;
    entity->logicPolicy = logicPolicy;
    entity->drawPolicy = drawPolicy;
    activeCount++;

    debug_log("Info: Entidade criada com sucesso!");
    return entity;
}

Entity* getEntity(u16 index) {
    if (index >= MAX_ENTITIES) return NULL;
    return &entityList[index];
}

void entity_destroy(Entity* entity) {
    if (!entity) return;
    
    u16 index = entity->index;
    
    // Limpa a entidade
    entity->body = NULL;
    entity->joyHandle = NULL;
    entity->onUpdate = NULL;
    entity->onDraw = NULL;
    entity->pData = NULL;
    entity->anim.sprite = NULL;
    
    // Adiciona à lista de livres
    entity->index = nextFree;
    nextFree = index;
    
    activeCount--;
}

void update_all_entities() {
    for (u16 i = 0; i < MAX_ENTITIES; i++) {
        Entity* e = &entityList[i];

        if (!e->active || !e->onUpdate) continue;

        Vect2D_s16 pos = entity_getGlobalPosition(e);
        AABB aabb; entity_getAABB(e, &aabb);

        if (should_update(&pos, &aabb, e->logicPolicy)) {
            e->onUpdate(e);
        }
    }
}

void entity_drawAll() {
    for (u16 i = 0; i < MAX_ENTITIES; i++) {
        Entity* e = &entityList[i];

        if (!e->active) continue;

        if (e->onDraw) {   
            e->onDraw(e);
            continue;
        } 
        
        if(!e->anim.sprite) continue;
        
        Vect2D_s16 pos = entity_getGlobalPosition(e);
        AABB aabb; entity_getAABB(e, &aabb);

        bool visible = should_update(&pos, &aabb, e->drawPolicy);
        entity_setVisible(e, visible);
        if (!visible) continue;
        
        s16 screenX = pos.x - camera_getPosition().x;
        s16 screenY = pos.y - camera_getPosition().y;
        SPR_setPosition(e->anim.sprite, screenX, screenY);        
    }
}

 void entity_setVisible(Entity* e, bool visible) {
    if (e->anim.visible != visible) {
        e->anim.visible = visible;
        SPR_setVisibility(e->anim.sprite, visible ? VISIBLE : HIDDEN);
    }
}

void interact_with(Entity* target) {
    if (target && target->onInteract) {
        target->onInteract(target);
    }
}

Vect2D_s16 entity_getGlobalPosition(Entity* e) {
    if (e->body) return e->body->globalPosition;

    if(e->tipo == ENTITY_TYPE_ITEM) 
        return (Vect2D_s16){((ItemDef*)e->pData)->hitbox.x, ((ItemDef*)e->pData)->hitbox.y};
    
    if(e->tipo == ENTITY_TYPE_NPC) 
        return (Vect2D_s16){((NpcSimpleDef*)e->pData)->hitbox.x, ((NpcSimpleDef*)e->pData)->hitbox.y};

    if(e->tipo == ENTITY_TYPE_TRIGGER) 
        return (Vect2D_s16){((TriggerDef*)e->pData)->hitbox.x, ((TriggerDef*)e->pData)->hitbox.y};

    return (Vect2D_s16){0, 0};
}

void entity_getAABB(Entity* e, AABB* out) {
    if (e->body) {
        *out = e->body->aabb;
    }else if(e->tipo == ENTITY_TYPE_ITEM){
        ItemDef* itemDef = (ItemDef*)e->pData;
        out = (AABB*)&itemDef->hitbox;
    }else if(e->tipo == ENTITY_TYPE_NPC){
        NpcSimpleDef* npcDef = (NpcSimpleDef*)e->pData;
        out = (AABB*)&npcDef->hitbox;
    }else if(e->tipo == ENTITY_TYPE_TRIGGER){
        TriggerDef* triggerDef = (TriggerDef*)e->pData;
        out = (AABB*)&triggerDef->hitbox;    
    }else {
        *out = (AABB){0};
    }
}