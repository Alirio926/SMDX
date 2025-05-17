#ifndef ANIM_CONTROLLER_H
#define ANIM_CONTROLLER_H

#include <genesis.h>
#include "core/game_config.h"

// Forward declarations
struct Entity;
struct PathAgent;
struct RigidBody;

// Conjunto de animações por estado
typedef struct {
    u16 idle;
    u16 walk;
    u16 run;
    u16 jump;
    u16 fall;
    u16 attack;
    u16 hurt;
    u16 climb;
} AnimStateSet;

extern const AnimStateSet defaultAnimSet;

// Controlador de animação
typedef struct {
    Sprite *sprite;
    bool visible;
    const AnimStateSet *animSet;
    u16 currentAnim;
} AnimController;

// Inicializa o controlador
void animcontroller_init(AnimController *ac, Sprite *sprite, const AnimStateSet *animSet);

// Atualiza a animação com base nos estados atuais
void animcontroller_update(AnimController* anim, struct RigidBody* body);

#endif // ANIM_CONTROLLER_H
