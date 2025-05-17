#ifndef ITEM_DEF_H
#define ITEM_DEF_H

#include <genesis.h>
#include "sprite.h"

struct Entity; // Forward declaration

typedef struct {
    const char* name;
    u16 id;
    const SpriteDefinition* sprite;
    Box hitbox;
    u16 flag_id;
    void (*onCollect)(struct Entity* self);
} ItemDef;

#endif // ITEM_DEF_H
