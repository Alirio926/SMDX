#ifndef PHYSIC_DEF_H
#define PHYSIC_DEF_H

// --- Layers (bitmasks para filtragem de colisão) ---
#define LAYER_PLAYER    0
#define LAYER_ENEMY     1
#define LAYER_PLATFORM  2
#define LAYER_ITEM      3
#define LAYER_TRIGGER   4
#define LAYER_SCENERY   5

#define FLAG_SOLID              (1 << 0)
#define FLAG_PUSHABLE           (1 << 1)
#define FLAG_IGNORE_GRAVITY     (1 << 2)
#define FLAG_NO_COLLISION       (1 << 3)
#define FLAG_CAN_RIDE           (1 << 4) // plataformas que o player pode "andar em cima"
#define FLAG_INTERACTABLE       (1 << 5)
#define FLAG_TRIGGER            (1 << 6)
// --- Masks ---
#define MASK_PLAYER       (1 << LAYER_PLATFORM | 1 << LAYER_ENEMY | 1 << LAYER_ITEM | 1 << LAYER_TRIGGER)
#define MASK_ENEMY        (1 << LAYER_PLAYER | 1 << LAYER_PLATFORM)
#define MASK_PLATFORM     (1 << LAYER_PLAYER) // plataformas só colidem com o player

// --- Tags ---
#define TAG_NOT_APPLICABLE      0
#define TAG_PLATFORM_NORMAL     1
#define TAG_PLATFORM_ONEWAY     2
#define TAG_PLATFORM_MOVING     3
#define TAG_PLATFORM_FALLING    4

// --- Tiles ---
#define TILE_EMPTY              0   // Sem colisão
#define TILE_SOLID              1   // Sólido completo
#define ONE_WAY_PLATFORM_TILE   2   // Plataforma que colide só por cima
#define SLOP_TILE               3   // Rampa (slope)
#define LADDER_TILE             4   // Escada (movimento vertical livre)

typedef struct {
    s16 x1, y1;
    s16 x2, y2;
    u8 type;
} Slope;

#endif