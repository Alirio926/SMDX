#include "player.h"
#include "sprite.h"
#include "maths.h"
#include "physics/physic_def.h"
#include "components/entity.h"
#include "components/rigidbody.h"
#include "physics/physic.h"
#include "core/update_policy.h"
#include "components/interact.h"
#include "core/logger.h"
#include "core/camera.h"
#include "core/game_config.h"

static void player_handleInput(u16 joy, u16 changed, u16 state);
static void update_player(Entity* self);
void player_onEvent(Entity* self, EntityEventType type);
static void debugDrawCollisionBoxes();

Sprite* Rect_1;
Sprite* Rect_2;
Sprite* Rect_3;
Sprite* Rect_4; 

static s8 x;
static Entity* pEntity;
s16 currentCoyoteTime;
s16 currentJumpBufferTime;
const s16 coyoteTime = 10;
const s16 jumpBufferTime = 10;
extern UpdateState g_gameState;

Entity* player_init(s16 x, s16 y){
    Entity* e = entity_create(UPDATE_ALWAYS, UPDATE_ALWAYS);
    RigidBody* body = rigidbody_create(UPDATE_ALWAYS, LAYER_PLAYER, MASK_PLAYER, TAG_NOT_APPLICABLE);

    body->position = newVector2D_s16(x, y);
    body->globalPosition = body->position;
    body->aabb = newAABB(8, 28, 0, 48);
    body->centerOffset = newVector2D_u16((body->aabb.min.x + body->aabb.max.x) / 2,
                                         (body->aabb.min.y + body->aabb.max.y) / 2);

    body->active = TRUE;
    body->collidable = TRUE;
    body->mState = MSTATE_IDLE;
    body->vState = VSTATE_AIRBORNE;
    body->aState = ASTATE_NONE;

    body->support = NULL;
    e->tipo = ENTITY_TYPE_PLAYER;
    e->flags = FLAG_SOLID;  

    e->onDraw = NULL;//player_draw;
    e->joyHandle = player_handleInput;
    e->onUpdate = update_player;   
    e->onEvent = player_onEvent;

    e->active = TRUE;
    e->body = body;

    e->pData = NULL;

    e->anim.sprite = SPR_addSprite(&player_sheet, x, y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    e->anim.visible = TRUE;
    animcontroller_init(&e->anim, e->anim.sprite, &defaultAnimSet);
    
    body->owner = e;
    pEntity = e;
    PAL_setPalette(PAL1, player_sheet.palette->data, DMA);

    //Rect_1 = SPR_addSprite(&spr_rect_bb, x, y, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
	//Rect_2 = SPR_addSprite(&spr_rect_bb, x, y, TILE_ATTR(PAL1, FALSE, FALSE, TRUE));
	//Rect_3 = SPR_addSprite(&spr_rect_bb, x, y, TILE_ATTR(PAL1, FALSE, TRUE, FALSE));
	//Rect_4 = SPR_addSprite(&spr_rect_bb, x, y, TILE_ATTR(PAL1, FALSE, TRUE, TRUE));

    return e;
}

// chamado via evento do joyhandler
static void player_handleInput(u16 joy, u16 changed, u16 state) {
    if(joy == JOY_1){
        if (state & BUTTON_RIGHT) {
            x = 1;
        } else if (state & BUTTON_LEFT) {
            x = -1;
        } else if (changed & (BUTTON_RIGHT | BUTTON_LEFT)) {
            x = 0;
        } 
                
        if (changed & (BUTTON_A)) {
			if (state & (BUTTON_A)) {
                currentJumpBufferTime = jumpBufferTime;
            }else if (pEntity->body->vState == VSTATE_JUMPING && pEntity->body->velocity.fixY < 0) {
				//If the button is released we remove half of the velocity
				pEntity->body->velocity.fixY = F16_mul(pEntity->body->velocity.fixY, FIX16(.5));
			}
        }else if (changed & (BUTTON_B)) {
			if (state & (BUTTON_B)) {
                tryInteract(pEntity);
            }
        }
        else if (changed & (BUTTON_C)) {
			if (state & (BUTTON_C)) {
                const  u16 X = 2, Y = 21, W = 33, H = 6;    
                char* txt = ("Este e' um exemplo de texto longo que "
                            "ultrapassa a altura da caixa, entao "
                            "ele vai rolar automaticamente.\n"
                            "Pode colocar '\\n' onde quiser.\n"
                            "E continua rolando ate' o fim.");
                dialogue_beginText(pEntity->dialogue, X, Y, W, H, txt);   
                g_gameState = GAME_DIALOGUE;             
            }
        }
    }
}   

static void update_player(Entity* self) {
    RigidBody* body = self->body;

    // --- Aceleração horizontal
    if (x < 0) {
        body->velocity.fixX -= body->physics->acceleration;
        if (body->velocity.fixX < -body->physics->maxRunSpeed) 
            body->velocity.fixX = -body->physics->maxRunSpeed;
    } else if (x > 0) {
        body->velocity.fixX += body->physics->acceleration;
        if (body->velocity.fixX > body->physics->maxRunSpeed) 
            body->velocity.fixX = body->physics->maxRunSpeed;
    } else if (body->vState == VSTATE_GROUNDED) {
        // --- Desaceleração automática se no chão e sem input
        if (body->velocity.fixX > 0) {
            body->velocity.fixX -= body->physics->deceleration;
            if (body->velocity.fixX < 0) body->velocity.fixX = 0;
        } else if (body->velocity.fixX < 0) {
            body->velocity.fixX += body->physics->deceleration;
            if (body->velocity.fixX > 0) body->velocity.fixX = 0;
        } else {
            body->velocity.fixX = 0;
        }
    }

    // Atualiza a versão inteira da velocidade horizontal
    body->velocity.x = clamp(F16_toInt(body->velocity.fixX), -body->physics->maxRunSpeed, body->physics->maxRunSpeed);

    // Verifica se deve pular
    if (currentCoyoteTime > 0 && currentJumpBufferTime > 0 && body->vState == VSTATE_GROUNDED) {
        //body->vState = VSTATE_JUMPING;
        physics_applyImpulse(body, 0, -body->physics->jumpSpeed);
        currentCoyoteTime = 0;
        currentJumpBufferTime = 0;
    }

    if(body->vState != VSTATE_GROUNDED)
        currentCoyoteTime--;
    else
        currentCoyoteTime = coyoteTime;

    currentJumpBufferTime = clamp((currentJumpBufferTime - 1), 0, jumpBufferTime); 

    animcontroller_update(&self->anim, body);
}

void player_onEvent(Entity* self, EntityEventType type) {
    if (type == ENTITY_EVENT_LAND) {
        debug_log("Player Info: landed");
    }
}

static void debugDrawCollisionBoxes() {
    /*SPR_setPosition(Rect_1, pEntity->body->position.x + pEntity->body->box.min.x, 
                            pEntity->body->position.y + pEntity->body->box.min.y);

    SPR_setPosition(Rect_2, pEntity->body->position.x + pEntity->body->box.max.x - 8, 
                            pEntity->body->position.y + pEntity->body->box.min.y);

    SPR_setPosition(Rect_3, pEntity->body->position.x + pEntity->body->box.min.x, 
                            pEntity->body->position.y + pEntity->body->box.max.y - 8);

    SPR_setPosition(Rect_4, pEntity->body->position.x + pEntity->body->box.max.x - 8, 
                            pEntity->body->position.y + pEntity->body->box.max.y - 8);*/
}