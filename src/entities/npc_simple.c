// npc_simple.c — NPCs estáticos com múltiplas falas (Engine-SGDK)
// -----------------------------------------------------------------------------
#include <genesis.h>
#include "components/entity.h"
#include "components/dialogue.h"
#include "npc_simple.h"
#include "core/anim_controller.h"
#include "physics/physic_def.h"
#include "core/logger.h"

extern SpriteDefinition npc_sheet;
extern const AnimStateSet npcAnimSet;

extern UpdateState g_gameState;

void npc_simple_onInteract(Entity* e)
{
    NpcSimpleDef* def = (NpcSimpleDef*)e->pData;
    if (def->textIndex >= def->textCount) return; // já terminou

    const char* text = def->texts[def->textIndex];
    dialogue_beginText(e->dialogue, 2, 21, 33, 6, text);
    g_gameState = GAME_DIALOGUE;

    switch (def->textMode) {
        case NPC_TEXTMODE_LOOP:
            def->textIndex = (def->textIndex + 1) % def->textCount;
            break;
        case NPC_TEXTMODE_STOP_LAST:
            if (def->textIndex < def->textCount - 1)
                def->textIndex++;
            break;
        case NPC_TEXTMODE_ADVANCE:
            def->textIndex++;
            break;
    }
}

Entity* npc_createSimple(const NpcSimpleDef* src)
{
    Entity* e = entity_create(UPDATE_DISABLED, UPDATE_VISIBLE_ONLY);
    if (!e) return NULL;

    NpcSimpleDef* def = MEM_alloc(sizeof(NpcSimpleDef));
    *def = *src; // copia a struct inteira
    def->textIndex = 0; // inicia no começo

    e->active = TRUE;
    e->tipo = ENTITY_TYPE_NPC;
    e->flags = FLAG_INTERACTABLE;
    e->pData = (void*)def;

    e->onInteract = npc_simple_onInteract;

    e->anim.animSet = &defaultAnimSet;
    e->anim.sprite = SPR_addSprite(&player_sheet, def->hitbox.x, def->hitbox.y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    e->anim.visible = TRUE;
    
    SPR_setPosition(e->anim.sprite, def->hitbox.x, def->hitbox.y);

    debug_log("Npc: Spawnado em: %d,%d", def->hitbox.x, def->hitbox.y);

    return e;
}
