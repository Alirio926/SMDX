/**
 * @file physic.c
 * @brief Sistema de física 2D para o jogo
 * 
 * Este arquivo implementa o sistema de física 2D, incluindo:
 * - Colisão com tiles
 * - Gravidade e movimento
 * - Suporte para plataformas móveis
 * - Colisão com rampas
 */

#include "physic.h"
#include "components/rigidbody.h"
#include "components/entity.h"
#include "core/camera.h"
#include "entities/player.h"
#include "physics/physic_def.h"
#include "tiled/tiled_map.h"
#include "core/logger.h"
#include "tiled/fase1_slopes.h"
#include "components/path_follower.h"
#include "core/game_config.h"
#include "components/blocking_zone.h"

#define SUPPORT_EPSILON 4  // tolerância de até 4px entre base e topo
#define FIX32_TO_TILE(x)  ((x) >> 10)  // (26.6 → 16.4 → /16)

void physics_checkGroundCollision(RigidBody* body);
void physics_checkWallCollision(RigidBody* body);
void physics_checkSlopeCollision(RigidBody* body);
void physics_checkStairsCollision(RigidBody* body);
void physics_checkCustomTiles(RigidBody* body); 
void physics_checkPlatformCollision(RigidBody* body);
s16 physics_getSlopeY(s16 x, const Slope* slope);
RigidBody* findSupportBelow(RigidBody* self);
bool aabb_checkVerticalSupport(const RigidBody* self, const RigidBody* other);

/**
 * @brief Inicializa o sistema de física
 */
void physics_init() {
    debug_log("Physics system init.");
}

/**
 * @brief Aplica um impulso (força instantânea) ao corpo
 * @param body Corpo rígido que receberá o impulso
 * @param dx Componente X do impulso
 * @param dy Componente Y do impulso
 */
void physics_applyImpulse(RigidBody* body, fix16 dx, fix16 dy) {
    body->velocity.fixY += dy;
}

/**
 * @brief Atualiza todos os corpos físicos do jogo
 * 
 * Esta função é chamada a cada frame e:
 * 1. Atualiza a posição de todos os corpos ativos
 * 2. Aplica gravidade
 * 3. Resolve colisões com tiles e outros corpos
 * 4. Atualiza a posição visual baseada na câmera
 */
void physics_updateAll() {
    bool update;
    for (u16 i = 0; i < MAX_BODIES; i++) {
        RigidBody* body = getRigidBody(i);
        Entity* e = body->owner;
        if(!body->active) continue;        
        
        // Atualiza gravidade, posição e delta!
        rigidbody_update(body);

        update = should_update(&body->globalPosition, &body->aabb, body->physicsPolicy);
        // Segue para fisica apenas se estiver com fisica habilidata
        if (!update) continue;

        // Só checa colisões se o corpo for colidível e não for plataforma
        if(body->collidable && (e->flags & FLAG_SOLID)) {
            // Checar colisões com tiles
            physics_checkWallCollision(body);
            physics_checkGroundCollision(body);
            physics_checkSlopeCollision(body);
            //physics_checkStairsCollision(body);
            //physics_checkCustomTiles(body);
            physics_checkPlatformCollision(body);
            physics_checkBlockingZones(body);
            // Verifica se o corpo está apoiado em algum outro corpo
            if(body->support && body->support->active ){
                // Verifica se os layers são compatíveis usando a máscara
                if(body->mask & (1 << body->support->layer)) {
                    s32 supportY = (body->support->globalPosition.y);            
                    s16 deltaY = supportY - (body->globalPosition.y + body->aabb.max.y);

                    if(deltaY >= -8 && deltaY <= 4 && body->velocity.fixY >= 0){
                        s16 heigth = body->aabb.max.y - body->aabb.min.y;
                        
                        body->globalPosition.y = supportY - heigth;                        
                        body->globalPosition.x += F16_toInt(body->support->delta.x);
                        body->velocity.fixY = 0;
                        body->vState = VSTATE_GROUNDED;
                    } 
                }
            }
        }

        if(body->owner){
            Entity* e = body->owner;
            if(body->vState == VSTATE_GROUNDED && !e->wasOnGround){ 
                e->onEvent(e, ENTITY_EVENT_LAND);
            }
            e->wasOnGround = (body->vState == VSTATE_GROUNDED);
        }
        // atualiza posição visual com base na câmera apos checar todas as colisions
        body->position.x = body->globalPosition.x - camera_getPosition().x;
        body->position.y = body->globalPosition.y - camera_getPosition().y;
    }
}

void physics_checkVHGroundCollision(RigidBody* body){
    if (!body->active || !body->collidable) return;

    //Create level limits
    AABB levelLimits = tilemap_getWorldRoomBounds();

    //Easy access to the bounds in global pos
    AABB bodyBounds = newAABB(
        body->globalPosition.x + body->aabb.min.x,
        body->globalPosition.x + body->aabb.max.x,
        body->globalPosition.y + body->aabb.min.y,
        body->globalPosition.y + body->aabb.max.y
    );

    //We can see this variables as a way to avoid thinking that a ground tile is a wall tile
    //Skin width (yIntVelocity) changes depending on the vertical velocity
    s16 yIntVelocity = F16_toRoundedInt(body->velocity.fixY);
    s16 playerHeadPos = body->aabb.min.y - yIntVelocity + body->globalPosition.y;
    s16 playerFeetPos = body->aabb.max.y - yIntVelocity + body->globalPosition.y;

    //Positions in tiles
    Vect2D_u16 minTilePos = tiledMap_posToTile(newVector2D_s16(bodyBounds.min.x, bodyBounds.min.y));
    Vect2D_u16 maxTilePos = tiledMap_posToTile(newVector2D_s16(bodyBounds.max.x, bodyBounds.max.y));

    //Used to limit how many tiles we have to check for collision
    Vect2D_u16 tileBoundDifference = newVector2D_u16(maxTilePos.x - minTilePos.x, maxTilePos.y - minTilePos.y);

    //First we check for horizontal collisions
    for (u16 i = 0; i <= tileBoundDifference.y; i++) {
        //Height position constant as a helper
        const u16 y = minTilePos.y + i;

        //Right position constant as a helper
        const u16 rx = maxTilePos.x;

        u16 rTileValue = tiledMap_getTile(rx, y);
        //After getting the tile value, we check if that is one of whom we can collide/trigger with horizontally
        if (rTileValue == TILE_SOLID) {
            AABB tileBounds = tiledMap_getTileBounds(rx, y);
            //Before taking that tile as a wall, we have to check if that is within the player hitbox, e.g. not seeing ground as a wall
            if (tileBounds.min.x < levelLimits.max.x && tileBounds.min.y < playerFeetPos && tileBounds.max.y > playerHeadPos) {
                levelLimits.max.x = tileBounds.min.x;
                break;
            }
        }

        //Left position constant as a helper
        const s16 lx = minTilePos.x;

        u16 lTileValue = tiledMap_getTile(lx, y);
        //We do the same here but for the left side
        if (lTileValue == TILE_SOLID) {
            AABB tileBounds = tiledMap_getTileBounds(lx, y);
            if (tileBounds.max.x > levelLimits.min.x && tileBounds.min.y < playerFeetPos && tileBounds.max.y > playerHeadPos) {
                levelLimits.min.x = tileBounds.max.x;
                break;
            }
        }
    }

    //After checking for horizontal positions we can modify the positions if the player is colliding
    if (levelLimits.min.x > bodyBounds.min.x) {
        body->globalPosition.x = levelLimits.min.x - body->aabb.min.x;
        body->velocity.fixX = 0;
        body->velocity.x = 0;
    }
    if (levelLimits.max.x < bodyBounds.max.x) {
        body->globalPosition.x = levelLimits.max.x - body->aabb.max.x;
        body->velocity.fixX = 0;
        body->velocity.x = 0;
    }

    //Then, we modify the player position so we can use them to check for vertical collisions
    bodyBounds = newAABB(
        body->globalPosition.x + body->aabb.min.x,
        body->globalPosition.x + body->aabb.max.x,
        body->globalPosition.y + body->aabb.min.y,
        body->globalPosition.y + body->aabb.max.y
    );

    //And do the same to the variables that are used to check for them
    minTilePos = tiledMap_posToTile(newVector2D_s16(bodyBounds.min.x, bodyBounds.min.y));
    maxTilePos = tiledMap_posToTile(newVector2D_s16(bodyBounds.max.x - 1, bodyBounds.max.y));
    tileBoundDifference = newVector2D_u16(maxTilePos.x - minTilePos.x, maxTilePos.y - minTilePos.y);

    //To avoid having troubles with player snapping to ground ignoring the upward velocity, we separate top and bottom collisions depending on the velocity
    if (yIntVelocity >= 0) {
        for (u16 i = 0; i <= tileBoundDifference.x; i++) {
            s16 x = minTilePos.x + i;
            u16 y = maxTilePos.y;

            //This is the exact same method that we use for horizontal collisions
            u16 bottomTileValue = tiledMap_getTile(x, y);
            if (bottomTileValue == TILE_SOLID || bottomTileValue == ONE_WAY_PLATFORM_TILE) {
                if (getTileRightEdge(x) == levelLimits.min.x || getTileLeftEdge(x) == levelLimits.max.x)
                    continue;

                u16 bottomEdgePos = getTileTopEdge(y);
                //The error correction is used to add some extra width pixels in case the player isn't high enough by just some of them
                if (bottomEdgePos < levelLimits.max.y && bottomEdgePos >= (playerFeetPos - ONE_WAY_PLATFORM_ERROR_CORRECTION)) {
                    levelLimits.max.y = bottomEdgePos;
                }
            }
        }
    } else {
        for (u16 i = 0; i <= tileBoundDifference.x; i++) {
            s16 x = minTilePos.x + i;
            u16 y = minTilePos.y;

            //And the same once again
            u16 topTileValue = tiledMap_getTile(x, y);
            if (topTileValue == TILE_SOLID) {
                if (getTileRightEdge(x) == levelLimits.min.x || getTileLeftEdge(x) == levelLimits.max.x)
                    continue;

                u16 upperEdgePos = getTileBottomEdge(y);
                if (upperEdgePos < levelLimits.max.y) {
                    levelLimits.min.y = upperEdgePos;
                    break;
                }
            }
        }
    }

    //Now we modify the player position and some properties if necessary
    if (levelLimits.min.y > bodyBounds.min.y) {
        body->globalPosition.y = levelLimits.min.y - body->aabb.min.y;
        body->velocity.fixY = 0;
    }
    if (levelLimits.max.y <= bodyBounds.max.y) {
        if (levelLimits.max.y == tilemap_getWorldRoomBounds().max.y) {
            body->vState = VSTATE_FALLING;
        } else {
            body->vState = VSTATE_GROUNDED;
            body->globalPosition.y = levelLimits.max.y - body->aabb.max.y;
            body->velocity.fixY = 0;
        }
    } else {
        body->vState = VSTATE_FALLING;
    }
}

/**
 * @brief Verifica e resolve colisões com o chão
 * @param body Corpo rígido a ser verificado
 * 
 * Esta função:
 * 1. Verifica colisão com tiles sólidos
 * 2. Verifica colisão com plataformas one-way
 * 3. Atualiza o estado vertical do corpo (GROUNDED/FALLING)
 * 4. Ajusta a posição para evitar penetração
 */
void physics_checkGroundCollision(RigidBody* body) {
    if (!body->active || !body->collidable) return;

    AABB levelLimits = tilemap_getWorldRoomBounds();

    // Get player bounds
    AABB playerBounds = newAABB(
        body->globalPosition.x + body->aabb.min.x,
        body->globalPosition.x + body->aabb.max.x,
        body->globalPosition.y + body->aabb.min.y,
        body->globalPosition.y + body->aabb.max.y
    );

    // Skin width changes depending on vertical velocity
    s16 yIntVelocity = F16_toRoundedInt(body->velocity.fixY);
    s16 playerHeadPos = body->aabb.min.y - yIntVelocity + body->globalPosition.y;
    s16 playerFeetPos = body->aabb.max.y - yIntVelocity + body->globalPosition.y;

    // Verificação normal para velocidades menores
    Vect2D_u16 minTilePos = tiledMap_posToTile(newVector2D_s16(playerBounds.min.x, playerBounds.min.y));
    Vect2D_u16 maxTilePos = tiledMap_posToTile(newVector2D_s16(playerBounds.max.x - 1, playerBounds.max.y));
    Vect2D_u16 tileBoundDifference = newVector2D_u16(maxTilePos.x - minTilePos.x, maxTilePos.y - minTilePos.y);

    // Check bottom collisions only if moving downward
    if (yIntVelocity >= 0) {
        for (u16 i = 0; i <= tileBoundDifference.x; i++) {
            s16 x = minTilePos.x + i;
            u16 y = maxTilePos.y;

            u16 tile = tiledMap_getTile(x, y);
            if (tile == TILE_SOLID || tile == ONE_WAY_PLATFORM_TILE || tile == SLOP_TILE) {
                // Skip if tile is part of a wall we're already colliding with
                if (getTileRightEdge(x) == levelLimits.min.x || getTileLeftEdge(x) == levelLimits.max.x)
                    continue;

                u16 bottomEdgePos = getTileTopEdge(y);
                if (bottomEdgePos < levelLimits.max.y && bottomEdgePos >= (playerFeetPos - ONE_WAY_PLATFORM_ERROR_CORRECTION)) {
                    levelLimits.max.y = bottomEdgePos;
                }
            }
        }
    } else {
        // Check top collisions when moving upward
        for (u16 i = 0; i <= tileBoundDifference.x; i++) {
            s16 x = minTilePos.x + i;
            u16 y = minTilePos.y;

            u16 tile = tiledMap_getTile(x, y);
            if (tile == TILE_SOLID || tile == SLOP_TILE) {
                if (getTileRightEdge(x) == levelLimits.min.x || getTileLeftEdge(x) == levelLimits.max.x)
                    continue;

                u16 upperEdgePos = getTileBottomEdge(y);
                if (upperEdgePos < levelLimits.max.y) {
                    levelLimits.min.y = upperEdgePos;
                    break;
                }
            }
        }
    }

    // Update position and state based on collisions
    if (levelLimits.min.y > playerHeadPos) {
        body->globalPosition.y = levelLimits.min.y - body->aabb.min.y;
        body->velocity.fixY = 0;
    }
    if (levelLimits.max.y <= playerBounds.max.y) {
        if (levelLimits.max.y == tilemap_getWorldRoomBounds().max.y) {
            body->vState = VSTATE_FALLING;
        } else {
            body->vState = VSTATE_GROUNDED;
            body->globalPosition.y = levelLimits.max.y - body->aabb.max.y;
            body->velocity.fixY = 0;
        }
    } else {
        body->vState = VSTATE_FALLING;
    }
}

/**
 * @brief Verifica e resolve colisões com paredes
 * @param body Corpo rígido a ser verificado
 * 
 * Esta função:
 * 1. Verifica colisão com tiles sólidos nas laterais
 * 2. Ajusta a posição para evitar penetração
 * 3. Zera a velocidade horizontal em caso de colisão
 */
void physics_checkWallCollision(RigidBody* body) {
    if (!body->active || !body->collidable) return;

    AABB levelLimits = tilemap_getWorldRoomBounds();

    // Get player bounds
    AABB playerBounds = newAABB(
        body->globalPosition.x + body->aabb.min.x,
        body->globalPosition.x + body->aabb.max.x,
        body->globalPosition.y + body->aabb.min.y,
        body->globalPosition.y + body->aabb.max.y
    );

    // Skin width changes depending on vertical velocity
    s16 yIntVelocity = F16_toRoundedInt(body->velocity.fixY);
    s16 playerHeadPos = body->aabb.min.y - yIntVelocity + body->globalPosition.y;
    s16 playerFeetPos = body->aabb.max.y - yIntVelocity + body->globalPosition.y;

    Vect2D_u16 minTilePos = tiledMap_posToTile(newVector2D_s16(playerBounds.min.x, playerBounds.min.y));
    Vect2D_u16 maxTilePos = tiledMap_posToTile(newVector2D_s16(playerBounds.max.x, playerBounds.max.y));
    Vect2D_u16 tileBoundDifference = newVector2D_u16(maxTilePos.x - minTilePos.x, maxTilePos.y - minTilePos.y);

    for (u16 i = 0; i <= tileBoundDifference.y; i++) {
        const u16 y = minTilePos.y + i;

        // Check right side
        const u16 rx = maxTilePos.x;
        u16 rTile = tiledMap_getTile(rx, y);
        if(rTile == SLOP_TILE){
            continue;
        }
        if (rTile == TILE_SOLID) {
            AABB bounds = tiledMap_getTileBounds(rx, y);
            if (bounds.min.x < levelLimits.max.x && bounds.min.y < playerFeetPos && bounds.max.y > playerHeadPos) {
                levelLimits.max.x = bounds.min.x;
                break;
            }
        }

        // Check left side
        const s16 lx = minTilePos.x;
        u16 lTile = tiledMap_getTile(lx, y);
        if(lTile == SLOP_TILE){
            continue;
        }
        if (lTile == TILE_SOLID) {
            AABB bounds = tiledMap_getTileBounds(lx, y);
            if (bounds.max.x > levelLimits.min.x && bounds.min.y < playerFeetPos && bounds.max.y > playerHeadPos) {
                levelLimits.min.x = bounds.max.x;
                break;
            }
        }
    }

    // Update position based on collisions
    if (levelLimits.min.x > playerBounds.min.x) {
        body->globalPosition.x = levelLimits.min.x - body->aabb.min.x;
        body->velocity.fixX = 0;
        body->velocity.x = 0;
    }
    if (levelLimits.max.x < playerBounds.max.x) {
        body->globalPosition.x = levelLimits.max.x - body->aabb.max.x;
        body->velocity.fixX = 0;
        body->velocity.x = 0;
    }
}

/**
 * @brief Verifica e resolve colisões com rampas
 * @param body Corpo rígido a ser verificado
 * 
 * Esta função:
 * 1. Verifica se o corpo está sobre uma rampa
 * 2. Ajusta a posição Y baseada na inclinação da rampa
 * 3. Aplica movimento horizontal baseado na inclinação
 */
void physics_checkSlopeCollision(RigidBody* body) {
    if (!body->active || !body->collidable) return;

    s16 px = body->globalPosition.x + body->centerOffset.x;
    s16 py = body->globalPosition.y + body->aabb.max.y;

    AABB* screenBounds = camera_getScreenBounds();

    for(u16 i = 0; i < SLOPE_COUNT; i++){
        const Slope* slope = &slopes[i];
        
        s16 sx1 = slope->x1;
        s16 sy1 = slope->y1;
        s16 sx2 = slope->x2;
        s16 sy2 = slope->y2;

        // Check if slope is visible
        if(sx2 < screenBounds->min.x || sx1 > screenBounds->max.x || 
           sy2 < screenBounds->min.y || sy1 > screenBounds->max.y) {
            continue; // Skip if slope is not visible
        }

        if(sx1 > sx2){
            SWAP_s16(sx1, sx2);
            SWAP_s16(sy1, sy2);
        }

        if(px >= sx1 && px <= sx2){
            s16 slopeY = physics_getSlopeY(px, slope);
            if(py >= slopeY - 6 && py <= slopeY + 8){
                body->globalPosition.y = slopeY - body->aabb.max.y;
                body->vState = VSTATE_GROUNDED;
                body->velocity.fixY = 0;

                s32 dx = slope->x2 - slope->x1;
                s32 dy = slope->y2 - slope->y1;

                if(dx != 0){
                    fix16 slopeRatio = F16_div(FIX16(dy), FIX16(dx));
                    body->globalPosition.x += FF16_toRoundedInt(slopeRatio);
                }
                break;
            }
        }
    }    
}

void physics_checkStairsCollision(RigidBody* body) {
    // Ainda vazio
}

void physics_checkCustomTiles(RigidBody* body) {
    // Ainda vazio
}

/**
 * @brief Verifica se um corpo está apoiado em uma plataforma móvel
 * @param body Corpo rígido a ser verificado
 * 
 * Esta função:
 * 1. Procura por plataformas abaixo do corpo
 * 2. Verifica se há sobreposição horizontal
 * 3. Verifica se o corpo está dentro da margem de suporte
 * 4. Atualiza o suporte do corpo se encontrado
 */
void physics_checkPlatformCollision(RigidBody* body) {
    body->support = findSupportBelow(body);
}

/**
 * @brief Encontra uma plataforma que pode suportar o corpo
 * @param self Corpo rígido que procura suporte
 * @return RigidBody* da plataforma suporte, ou NULL se não encontrada
 */
RigidBody* findSupportBelow(RigidBody* self) {
    for (u16 i = 0; i < MAX_ENTITIES; i++) {
        Entity* e = getEntity(i);
        if (!e->active || !e->body) continue;

        // Opcional: filtrar apenas plataformas móveis ou sólidas
        if (e->tipo != ENTITY_TYPE_PLATFORM) continue;
        
        if (aabb_checkVerticalSupport(self, e->body)) {
            return e->body;
        }
    }
    return NULL;
}

#define SUPPORT_EPSILON 4  // tolerância de até 4px entre base e topo
#define SUPPORT_MARGIN  6

/**
 * @brief Verifica se um corpo está apoiado verticalmente em outro
 * @param self Corpo rígido que procura suporte
 * @param other Corpo rígido potencialmente suporte
 * @return true se other pode suportar self
 */
bool aabb_checkVerticalSupport(const RigidBody* self, const RigidBody* other) {
    Entity* o = other->owner;
    if (!self || !other) return FALSE;
    if (!self->active || !other->active) return FALSE;
    if (!self->collidable || !other->collidable) return FALSE;

    // Verifica se os layers são compatíveis usando a máscara
    if (!(self->mask & (1 << other->layer))) {
        return FALSE;
    }
    
    if (!(o->flags & FLAG_CAN_RIDE)) return FALSE;
    
    // Pega AABB global
    s16 selfLeft   = self->globalPosition.x + self->aabb.min.x;
    s16 selfRight  = self->globalPosition.x + self->aabb.max.x;
    s16 selfBottom = self->globalPosition.y + self->aabb.max.y;

    s16 otherLeft  = other->globalPosition.x + other->aabb.min.x;
    s16 otherRight = other->globalPosition.x + other->aabb.max.x;
    s16 otherTop   = other->globalPosition.y + other->aabb.min.y;

    // Verifica sobreposição horizontal
    bool overlapX = (selfRight > otherLeft) && (selfLeft < otherRight);

    // Verifica se está em cima
    bool isAbove = (selfBottom >= otherTop - SUPPORT_MARGIN) && (selfBottom <= otherTop + SUPPORT_EPSILON);

    return overlapX && isAbove;
}

/**
 * @brief Verifica se dois AABBs se intersectam
 * @param a Primeiro AABB
 * @param b Segundo AABB
 * @return true se os AABBs se intersectam
 */
bool aabb_intersect(const AABB* a, const AABB* b) {
    return (
        a->max.x > b->min.x &&
        a->min.x < b->max.x &&
        a->max.y > b->min.y &&
        a->min.y < b->max.y
    );
}

void rigidbody_getGlobalAABB(RigidBody* body, AABB* out) {
    s16 x = body->globalPosition.x;
    s16 y = body->globalPosition.y;
    out->min.x = x + body->aabb.min.x;
    out->min.y = y + body->aabb.min.y;
    out->max.x = x + body->aabb.max.x;
    out->max.y = y + body->aabb.max.y;
}

bool aabb_intersectBox(const Box* box, const AABB* aabb) {
    s16 ax1 = aabb->min.x;
    s16 ay1 = aabb->min.y;
    s16 ax2 = aabb->max.x;
    s16 ay2 = aabb->max.y;

    s16 bx1 = box->x;
    s16 by1 = box->y;
    s16 bx2 = box->x + box->w;
    s16 by2 = box->y + box->h;

    return (ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1);
}

bool box_intersect(const Box* a, const Box* b) {
    return (a->x < b->x + b->w &&
            a->x + a->w > b->x &&
            a->y < b->y + b->h &&
            a->y + a->h > b->y);
}

/**
 * @brief Calcula a posição Y em uma rampa para uma dada posição X
 * @param x Posição X para calcular Y
 * @param slope Rampa a ser usada no cálculo
 * @return Posição Y na rampa
 */
s16 physics_getSlopeY(s16 x, const Slope* slope){
    s16 x1 = slope->x1;
    s16 y1 = slope->y1;
    s16 x2 = slope->x2;
    s16 y2 = slope->y2;

    if(x1 > x2){
        SWAP_s16(x1, x2);
        SWAP_s16(y1, y2);
    }
    
    if(x <= x1) return y1;    
    if(x >= x2) return y2;
    
    s32 dx = (s32)(x2 - x1);
    s32 dy = (s32)(y2 - y1);
    s32 posX = (s32)(x -x1);

    s32 result = y1 + (dy * posX) / dx;
    return (s16)result; 
}

/*
// Função para calcular os tiles ocupados pela caixa de colisão
TileBounds getTileBounds(const RigidBody* body) {
    TileBounds t;

    s16 worldX = body->position.x - body->centerOffset.x;
    s16 worldY = body->position.y - body->centerOffset.y;

    t.leftX  = worldX + body->box.min.x;
    t.rightX = worldX + body->box.max.x - 1;
    t.topY   = worldY + body->box.min.y;
    t.bottomY = worldY + body->box.max.y - 1;

    t.left   = t.leftX >> 4;
    t.right  = t.rightX >> 4;
    t.top    = t.topY >> 4;
    t.bottom = t.bottomY >> 4;

    t.footY = worldY + body->box.max.y - 1;
    t.leftFootX = t.leftX;
    t.rightFootX = t.rightX;

    t.midY = (t.topY + t.bottomY) / 2;

    return t;
}*/