#include "anim_controller.h"
#include "components/entity_def.h"

void anim_update(AnimController* anim, RigidBody* body);

void animcontroller_init(AnimController *ac, Sprite *sprite, const AnimStateSet *animSet) {
    ac->sprite = sprite;
    ac->animSet = animSet;
    ac->currentAnim = 0xFFFF; // Nenhuma animação ativa no início
}

void animcontroller_update(AnimController* anim, struct RigidBody* body){
    u16 an = anim->animSet->idle; // Padrão: idle
    fix16 vx = body->velocity.x;
    fix16 vy = body->velocity.fixY;

    if(vy < 0)
        body->vState = VSTATE_FALLING;
    else if(vy > 0)
        body->vState = VSTATE_JUMPING;
    else
        body->vState = VSTATE_GROUNDED;

    if (vx > 0) {
        body->mState = MSTATE_RUNNING;
        SPR_setHFlip(anim->sprite, FALSE); // virado para direita
    } else if (vx < 0) {
        body->mState = MSTATE_RUNNING;
        SPR_setHFlip(anim->sprite, TRUE);  // virado para esquerda
    }else
        body->mState = MSTATE_IDLE;

    // Prioridade: ação > estado vertical > movimento
    if (body->aState == ASTATE_ATTACKING)      an = anim->animSet->attack;
    else if (body->aState == ASTATE_HURT)      an = anim->animSet->hurt;
    else if (body->aState == ASTATE_CLIMBING)  an = anim->animSet->climb;
    else if (body->vState == VSTATE_JUMPING)   an = anim->animSet->jump;
    else if (body->vState == VSTATE_FALLING)   an = anim->animSet->fall;
    else if (body->mState == MSTATE_RUNNING)   an = anim->animSet->run;
    else if (body->mState == MSTATE_WALKING)   an = anim->animSet->walk;

    if (an != anim->currentAnim) {
        anim->currentAnim = an;
        SPR_setAnim(anim->sprite, an);
    }
}