#ifndef OBJETOS_H
#define OBJETOS_H

#include <genesis.h>
#include "components/path_def.h"

extern const Vect2D_u16 path_34[];
extern const Vect2D_u16 path_43[];
extern const Vect2D_u16 path_51[];
typedef struct {
} player_start_pos_t;

extern const player_start_pos_t player_start_poss[];
typedef struct {
    u8 id;
    char* nome;
    u8 property;
    u8 type;
} Item_t;

extern const Item_t Items[];
typedef struct {
    u8 id;
    char* nome;
    bool status;
} enemy_t;

extern const enemy_t enemys[];
typedef struct {
    u8 id;
} Portal_t;

extern const Portal_t Portals[];
typedef struct {
    bool status;
} spawn_sprite_t;

extern const spawn_sprite_t spawn_sprites[];
typedef struct {
    PathAgentDef agentDef;
} platform_t;

extern const platform_t platforms[];
typedef struct {
    PathAgentDef agentDef;
} npc_t;

extern const npc_t npcs[];

#endif // OBJETOS_H
