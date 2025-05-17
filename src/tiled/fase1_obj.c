#include "fase1_obj.h"

const Vect2D_u16 path_34[] = {
    { 109, 354 },
    { 234, 354 },
};

const Vect2D_u16 path_43[] = {
    { 416, 551 },
    { 528, 551 },
    { 528, 487 },
};

const Vect2D_u16 path_51[] = {
    { 87, 584 },
    { 321, 585 },
};

const player_start_pos_t player_start_poss[] = {
    {  },
};

const Item_t Items[] = {
    { .id = 4, .nome = "Espada", .property = 4, .type = 0 },
    { .id = 4, .nome = "Espada2", .property = 4, .type = 0 },
};

const enemy_t enemys[] = {
    { .id = 60, .nome = "Mago Supremo", .status = true },
    { .id = 61, .nome = "Mago Supremo", .status = true },
};

const Portal_t Portals[] = {
    { .id = 1 },
};

const spawn_sprite_t spawn_sprites[] = {
    { .status = true },
    { .status = true },
    { .status = true },
};

const platform_t platforms[] = {
    { .agentDef = { .position = { 416, 551 }, .path = path_43, .path_len = 3, .delayInicial = 60, .path_mode = PATHMODE_LOOP, .velocidade = FIX16(1), .sprite = "spr_platform", .w = 80, .h = 16 } },
    { .agentDef = { .position = { 235, 310 }, .path = path_34, .path_len = 2, .delayInicial = 60, .path_mode = PATHMODE_PINGPONG, .velocidade = FIX16(1), .sprite = "spr_platform", .w = 80, .h = 16 } },
};

const npc_t npcs[] = {
    { .agentDef = { .position = { 168, 560 }, .path = path_51, .path_len = 2, .delayInicial = 60, .path_mode = PATHMODE_PINGPONG, .velocidade = FIX16(1), .sprite = "spr_platform", .w = 80, .h = 16 } },
};

