// tiled_map.h
#ifndef TILED_MAP_H
#define TILED_MAP_H

#include "types.h"
#include "xtypes.h"

// Estrutura de array linear
typedef struct {
    const u8* data;
    u16 width;
    u16 height;
    bool compressed;
} CollisionArray;

void tiledMap_loadFromArray(const CollisionArray* map);
void tiledMap_free();
u16 tiledMap_getHeight();
u16 tiledMap_getWidth();

u16 tiledMap_getTile(s16 tileX, s16 tileY) ;
Vect2D_u16 tiledMap_posToTile(Vect2D_s16 position);
AABB tiledMap_getTileBounds(u16 tileX, u16 tileY);
bool tiledMap_isSolid(u16 tileX, u16 tileY);
bool isTileSolidAtWorld(s16 worldX, s16 worldY) ;
// Checagem contra rigidbody
bool tiledMap_collidesWithRigidBody(RigidBody* body);

AABB tilemap_getWorldRoomBounds(void);

u16 getTileLeftEdge(u16 x);
u16 getTileRightEdge(u16 x);
u16 getTileTopEdge(u16 y);
u16 getTileBottomEdge(u16 y);
#endif // TILED_MAP_H