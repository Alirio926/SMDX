#include "trigger.h"
#include "components/entity.h"
#include "core/logger.h"
#include "physics/physic.h"
#include "components/blocking_zone.h"
#include "core/camera.h"

void trigger_draw(Entity* self);
void trigger_update(Entity* self);
void trigger_applyZoneAction(TriggerDef* def);

void myTriggerCallback(TriggerDef* def) {
    debug_log("Trigger ativado!");
}

void onEnterCallback(TriggerDef* def) {
    debug_log("Trigger onEnter!");
}

void onExitCallback(TriggerDef* def) {
    debug_log("Trigger onExit!");
}

static Entity* target = NULL;

void trigger_setTarget(Entity* p) {
    target = p;
}

Entity* entity_spawnTrigger(u16 index, Vect2D_u16 position, TriggerType triggerType, TriggerZoneAction zoneAction, TriggerCallback onEnter, TriggerCallback onExit, SpriteDefinition* spriteDef, BlockingZone* zone) {
    Entity* e = entity_create(UPDATE_VISIBLE_ONLY, UPDATE_VISIBLE_ONLY);
    if (!e) return NULL;

    // obj do tiled
    TriggerDef def = {
        .hitbox = {position.x, position.y, spriteDef->w, spriteDef->h},
        .type = triggerType,
        .triggered = FALSE,
        .zoneAction = zoneAction,
        .onEnter = onEnter,
        .onExit = onExit,
        .context = (void*)zone
    };

    e->active = TRUE;
    e->tipo = ENTITY_TYPE_TRIGGER;
    e->flags = FLAG_TRIGGER;

    TriggerDef* triggerDef = (TriggerDef*)MEM_alloc(sizeof(TriggerDef));
    triggerDef->hitbox.x = def.hitbox.x;
    triggerDef->hitbox.y = def.hitbox.y;
    triggerDef->hitbox.w = def.hitbox.w;
    triggerDef->hitbox.h = def.hitbox.h;
    triggerDef->triggered = FALSE;
    triggerDef->type = def.type;
    triggerDef->zoneAction = def.zoneAction;
    triggerDef->context = (void*)zone;
    triggerDef->onEnter = onEnter;
    triggerDef->onExit = onExit;

    AnimController anim = {.sprite = NULL, .visible = TRUE, .animSet = &defaultAnimSet}; 
    if(spriteDef) 
        anim.sprite =  SPR_addSprite(spriteDef, def.hitbox.x, def.hitbox.y, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    
    e->anim = anim;

    e->onInteract = NULL;
    e->onUpdate = trigger_update;
    e->onDraw = trigger_draw;
    e->onDestroy = NULL;
    e->onEndPath = NULL;
    e->onEvent = NULL;

    if(e->anim.sprite)
    SPR_setPosition(e->anim.sprite, def.hitbox.x, def.hitbox.y);

    debug_log("Trigger: Spawnado em: %d,%d", triggerDef->hitbox.x, triggerDef->hitbox.y);

    e->pData = triggerDef; 
    
    return e;
}

void trigger_update(Entity* self) {
   
    if (!self->active) return;
    if (!target || !target->body) return;

    AABB globalAABB;
    rigidbody_getGlobalAABB(target->body, &globalAABB);

    TriggerDef* triggerDef = (TriggerDef*)self->pData;
    bool hit = aabb_intersectBox(&triggerDef->hitbox, &globalAABB);
    switch (triggerDef->type) {
        case TRIGGER_TYPE_ONCE:
            if (!triggerDef->triggered && hit && triggerDef->onEnter) {
                triggerDef->onEnter(triggerDef);
                triggerDef->triggered = TRUE;
            }
            break;

        case TRIGGER_TYPE_REPEAT:
            if (hit && !triggerDef->triggered && triggerDef->onEnter) {
                triggerDef->onEnter(triggerDef);
                triggerDef->triggered = TRUE;
            }else if (!hit && triggerDef->triggered) {
                triggerDef->triggered = FALSE;
            }
            break;

        case TRIGGER_TYPE_ENTER_EXIT:
            if (hit && !triggerDef->triggered && triggerDef->onEnter) {
                triggerDef->onEnter(triggerDef);
                triggerDef->triggered = TRUE;
            } else if (!hit && triggerDef->triggered && triggerDef->onExit) {
                triggerDef->onExit(triggerDef);
                triggerDef->triggered = FALSE;
            }
            break;
    }
}

void trigger_draw(Entity* self) {
    TriggerDef* def = (TriggerDef*)self->pData;
    BlockingZone* zone = (BlockingZone*)def->context;

    Vect2D_s16 pos = entity_getGlobalPosition(self);
    AABB aabb; entity_getAABB(self, &aabb);

    bool visible = should_update(&pos, &aabb, self->drawPolicy);
    entity_setVisible(self, visible);
    if (visible){
        s16 screenX = pos.x - camera_getPosition().x;
        s16 screenY = pos.y - camera_getPosition().y;
        SPR_setPosition(self->anim.sprite, screenX, screenY);    
    }    

    if (!zone || !zone->sprite || !zone->active) return;

    s16 dx = zone->hitbox.x - camera_getPosition().x;
    s16 dy = zone->hitbox.y - camera_getPosition().y;

    if (dx + zone->hitbox.w >= 0 && dx <= 320 &&
        dy + zone->hitbox.h >= 0 && dy <= 224) {
        blocking_setVisible(zone, TRUE);        
    }
    else {
        blocking_setVisible(zone, FALSE);
    }   

    if(zone->visible) {
        SPR_setPosition(zone->sprite, dx, dy);
    }
}

void trigger_applyZoneAction_wrapper(void* ctx) {
    trigger_applyZoneAction((TriggerDef*)ctx);
}

void trigger_applyZoneAction(TriggerDef* def) {
    BlockingZone* zone = (BlockingZone*)def->context;
    if (!def || !zone) return;

    switch (def->zoneAction) {
        case TRIGGER_ZONE_ENABLE:
            blocking_enable(zone);
            break;

        case TRIGGER_ZONE_DISABLE:
            blocking_disable(zone);
            break;

        case TRIGGER_ZONE_TOGGLE:
            zone->active ? blocking_disable(zone)
                         : blocking_enable(zone);
            break;

        default:
            break;
    }

    debug_log("Trigger: Aplicando acao da zona! %d", zone->active);
}