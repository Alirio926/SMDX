// tiled_map.c
#include "components/rigidbody_def.h"
#include "physics/physic.h"
#include "tiled_map.h"
#include "physics/physic_def.h"

static u8** currentMap = NULL;
static Vect2D_u16 mapSize;

void decompressRLE(const u8* src, u8* dst, u16 expectedSize) {
    u16 si = 0;
    u16 di = 0;

    while (di < expectedSize) {
        u8 b = src[si++];
        if (b & 0x80) {
            u8 count = (b & 0x7F) + 1;
            u8 value = src[si++];
            for (u8 i = 0; i < count; i++) dst[di++] = value;
        } else {
            dst[di++] = b;
        }
    }
}

void tiledMap_loadFromArray(const CollisionArray* map) {
    mapSize.x = map->width;
    mapSize.y = map->height;

    currentMap = MEM_alloc(mapSize.y * sizeof(u8*));

    u16 total = mapSize.x * mapSize.y;

    const u8* source = NULL;
    u8* tempBuffer = NULL;

    if (map->compressed) {
        tempBuffer = MEM_alloc(total);
        decompressRLE(map->data, tempBuffer, total);
        source = tempBuffer;
    } else {
        source = map->data;
    }

    for (u16 y = 0; y < mapSize.y; y++) {
        currentMap[y] = MEM_alloc(mapSize.x);
        memcpy(currentMap[y], &source[y * mapSize.x], mapSize.x);
    }

    if (tempBuffer) {
        MEM_free(tempBuffer);
    }
}

u16 tiledMap_getWidth(){return mapSize.x;}
u16 tiledMap_getHeight(){return mapSize.y;}

void tiledMap_free() {
    if (!currentMap) return;
    for (u16 i = 0; i < mapSize.y; i++) {
        MEM_free(currentMap[i]);
    }
    MEM_free(currentMap);
    currentMap = NULL;
}

bool isTileSolidAtWorld(s16 worldX, s16 worldY) {
    s16 tileX = worldX >> 4;
    s16 tileY = worldY >> 4;
    return (tiledMap_getTile(tileX, tileY) == TILE_SOLID);
}

bool tiledMap_isSolid(u16 tileX, u16 tileY){
    return tiledMap_getTile(tileX, tileY) == TILE_SOLID;
}

u16 tiledMap_getTile(s16 tileX, s16 tileY) {
    if (tileX >= mapSize.x || tileY >= mapSize.y || tileX < 0 || tileY < 0) return TILE_EMPTY;
    
    return currentMap[tileY][tileX];
}

Vect2D_u16 tiledMap_posToTile(Vect2D_s16 position) {
    return newVector2D_u16(position.x >> 4, position.y >> 4);
}

AABB tiledMap_getTileBounds(u16 tileX, u16 tileY) {
    return newAABB(tileX << 4, (tileX << 4) + 16, tileY << 4, (tileY << 4) + 16);
}

bool tiledMap_collidesWithRigidBody(RigidBody* body) {
    if (!body || !body->collidable || !body->active) return FALSE;

    AABB bounds = body->aabb;

    Vect2D_u16 tileMin = newVector2D_u16(bounds.min.x >> 4, bounds.min.y >> 4);
    Vect2D_u16 tileMax = newVector2D_u16(bounds.max.x >> 4, bounds.max.y >> 4);

    for (u16 y = tileMin.y; y <= tileMax.y; y++) {
        for (u16 x = tileMin.x; x <= tileMax.x; x++) {
            if (tiledMap_isSolid(x, y)) {
                AABB tileBox = tiledMap_getTileBounds(x, y);
                if (aabb_intersect(&bounds, &tileBox)) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

AABB tilemap_getWorldRoomBounds(void) {
    return newAABB(0, mapSize.x << 4, 0, mapSize.y << 4);
}

//Used to get the left edge of a tile by inputting the tile position
u16 getTileLeftEdge(u16 x) {
	return (x << 4);
}
//Used to get the right edge of a tile by inputting the tile position
u16 getTileRightEdge(u16 x) {
	return (x << 4) + 16;
}
//Used to get the top edge of a tile by inputting the tile position
u16 getTileTopEdge(u16 y) {
	return (y << 4);
}
//Used to get the bottom edge of a tile by inputting the tile position
u16 getTileBottomEdge(u16 y) {
	return (y << 4) + 16;
}