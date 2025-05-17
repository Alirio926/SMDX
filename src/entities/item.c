#include "item.h"
#include "components/entity.h"
#include "core/logger.h"
#include "core/camera.h"
#include "physics/physic_def.h"

void update_item(Entity* self) ;


Entity* entity_spawnItem(u16 index, const ItemDef* def) {
    Entity* e = entity_create(UPDATE_DISABLED, UPDATE_VISIBLE_ONLY);
    if (!e) return NULL;

    e->active = TRUE;
    e->tipo = ENTITY_TYPE_ITEM;
    e->flags = FLAG_INTERACTABLE;
    e->pData = (void*)def;

    // Criar hitbox
    ItemDef* itemDef = (ItemDef*)MEM_alloc(sizeof(ItemDef));
    itemDef->hitbox.x = def->hitbox.x;
    itemDef->hitbox.y = def->hitbox.y;
    itemDef->hitbox.w = def->hitbox.w;
    itemDef->hitbox.h = def->hitbox.h;
    
    debug_log("Item: Spawnado em: %d,%d", itemDef->hitbox.x, itemDef->hitbox.y);

    e->onUpdate = NULL;

    // Inicializa animação
    // --- SPRITE ---
    e->anim.animSet = NULL;
    e->anim.sprite = SPR_addSprite(&spr_platform, itemDef->hitbox.x, itemDef->hitbox.y, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
    e->anim.visible = true;
    PAL_setPalette(PAL2, spr_platform.palette->data, DMA);

    // Coloca na posição inicial
    SPR_setPosition(e->anim.sprite, itemDef->hitbox.x, itemDef->hitbox.y);

    // Define função de interação
    e->onInteract = def->onCollect ? def->onCollect : coletar_item;

    e->pData = itemDef;
    debug_log("Info: Item criado com sucesso!");
    return e;
}

void coletar_item(Entity* self) {
    //ItemDef* def = (ItemDef*)self->pData;
    //setFlag(def->flag_id);

    SPR_setVisibility(self->anim.sprite, HIDDEN);
    self->active = FALSE;

    debug_log("Item coletado!");
}
