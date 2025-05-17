#include <genesis.h>
#include <string.h>
#include "game.h"
#include "core/engine.h"
#include "gfx.h"
#include "tiled/fase1_col.h"
#include "tiled/fase1_obj.h"
#include "components/path_def.h"
#include "physics/physic_def.h"
#include "components/object_factory.h"
#include "entities/item.h"
#include "core/logger.h"
#include "components/trigger.h"
#include "components/blocking_zone.h"
#include "components/dialogue.h"
#include "types.h"
#include "entities/npc_simple.h"

static Map* fase1_bga;
static Entity* entityPlayer;
//Index of the last tile that has been placed, useful to avoid overlapping
static u16 VDPTilesFilled = TILE_USER_INDEX;

DialogueState activeDialogue;
UpdateState g_gameState;

void GameInit(){

    VDP_init();
    SPR_init();

    VDP_loadTileSet(&bga, VDPTilesFilled, DMA);
    PAL_setPalette(PAL0, bga_pal.data, DMA);  
    fase1_bga = MAP_create(&bga_map, BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, VDPTilesFilled)); 
    VDPTilesFilled += bga.numTile;
    // carrega array de colisão exportado do tiled
    tiledMap_loadFromArray(&fase1_col);
    
    // inicia sistema de entity
    init_entity();
    // inicia sistema de fisica
    physics_init();
    // inicia sistema de rigidbody
    rigidbody_init();
    // inicia player na pos x e y
    entityPlayer = player_init(600, 520);

    Vect2D_u16 deadzone = { 64, 48 };
    // inicia camera
    camera_init(entityPlayer->body, deadzone);
    // linka bga com camera
    camera_bindMaps(fase1_bga, NULL);
    
    blocking_clearAll();
    
    spawn_entity_platform(0, DIST_EUCLIDEAN);

    ItemDef item_def = {
        .name = "Item de teste",
        .id = 0,
        .sprite = &spr_platform,
        .flag_id = 0,
        .hitbox = {
            .x = 750,
            .y = 470,
            .w = 80,
            .h = 16
        },
        .onCollect = NULL
    };

    entity_spawnItem(0, &item_def);

    create_trigger_with_blocking_zone(
        newVector2D_u16(350, 545), newVector2D_u16(750, 480), 
        &spr_totem, &spr_platformV, 
        TRIGGER_ZONE_TOGGLE, TRIGGER_TYPE_REPEAT,
        NULL, trigger_applyZoneAction_wrapper, NULL);

    trigger_setTarget(entityPlayer);

    /* --------------------------------------- */
    VDP_loadFont(custom_font.tileset, DMA);
    dialogue_init(&activeDialogue, PAL1, VDPTilesFilled);
    dialogue_setFont(&activeDialogue, &custom_font, PAL1);    
    
    NpcSimpleDef tia = {
        .hitbox = { 450, 560, 32, 64 },
        .texts = { "Oi, querido!", "Aproveite sua jornada.", "Passe em casa depois." },
        .textCount = 3,
        .textMode = NPC_TEXTMODE_STOP_LAST
    };

    npc_createSimple(&tia);

    debug_log("Info: GameInit finalizado com sucesso!");
}

void GameUpdate(void){
    if (g_gameState == GAME_DIALOGUE)
    {
        if (dialogue_updateText(&activeDialogue))
        {
            if (JOY_readJoypad(0) & BUTTON_START)
            {
                // Fecha a caixa e volta ao jogo
                dialogue_boxCloseCenter(&activeDialogue, 
                                        activeDialogue.bgX, 
                                        activeDialogue.bgY, 
                                        activeDialogue.bgW, 
                                        activeDialogue.bgH, 
                                        activeDialogue.boxBaseTile, 
                                        activeDialogue.fontPalette, 2);
                g_gameState = GAME_RUNNING;
            }
        }

        // Mesmo em diálogo, ainda pode:
        // - atualizar animações de cenário
        // - atualizar câmeras
        camera_update();
        entity_drawAll();
        SPR_update();
        SYS_doVBlankProcess();
        return;
    }

    /* Responsável por: Atualizar a lógica da entidade (input, IA, timers etc.)
     * Aqui cada entidade (player, inimigo, objeto) roda seu update(self) individual.
     * entity.h chama update da entidade
     */ 
    update_all_entities();
    
    /* Responsável por: Aplicar movimentação física, colisão com o mundo e entre corpos
     * Aqui você percorre os RigidBody registrados. Chama rigidbody_update(...)
     * physic.h
     */
    physics_updateAll();    

    /* Responsável por: Atualizar posição da camera
    */
    camera_update();

    /* Responsável por: Atualizar visual (sprite) de cada entidade
     * Atualiza a animação (frame atual), sincroniza a posição do Sprite com RigidBody.position
     * entity.h chama draw da entidade
     */
    entity_drawAll();

    SPR_update();
    SYS_doVBlankProcess();
}

void GameHandlerInput(u16 joy, u16 changed, u16 state){
    // Pode passar o InputHandler para o player
    if (entityPlayer->joyHandle)
        entityPlayer->joyHandle(joy, changed, state);
    else{
        // Ou tratar aqui mesmo.


    }
}

void GameUnloadState(){

}
