#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "types.h"
#include "core/anim_controller.h"

// Converte posição fixa (<<6) para inteiro (pixel)
#define POS_TO_INT(x)   ((x) >> 6)
// Converte pixel para posição fixa (<<6)
#define INT_TO_POS(x)   ((x) << 6)

// Parâmetros físicos do jogador
typedef struct {
    fix16 gravity;
    fix16 acceleration;
    fix16 deceleration;
    fix16 maxFallSpeed;
    fix16 jumpSpeed;
    fix16 maxRunSpeed;
} EntityPhysicsParams;

typedef enum {
    GAME_RUNNING,
    GAME_DIALOGUE
} UpdateState;

// Instância global
extern EntityPhysicsParams defaultPhysicsParams;

typedef enum VerticalState {
    VSTATE_GROUNDED,   // Está tocando o chão
    VSTATE_JUMPING,    // Está subindo após o pulo
    VSTATE_FALLING,    // Está caindo (sem suporte embaixo)
    VSTATE_AIRBORNE    // Em transição ou estado indefinido (pode ser usado como fallback)
} VerticalState;

typedef enum MovementState {
    MSTATE_IDLE,       // Parado
    MSTATE_WALKING,    // Andando
    MSTATE_RUNNING,    // Correndo
    MSTATE_DASHING     // Dash (se aplicável)
} MovementState;

typedef enum ActionState {
    ASTATE_NONE,        // Nenhuma ação especial
    ASTATE_ATTACKING,   // Atacando
    ASTATE_CASTING,     // Usando magia
    ASTATE_INTERACTING, // Interagindo com algo (ex: NPC, alavanca)
    ASTATE_HURT,        // Sofrendo dano
    ASTATE_CLIMBING     // Subindo escada ou parede
} ActionState;

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 224
#define MAX_ENTITIES 32
#define MAX_BODIES   16
#define ONE_WAY_PLATFORM_ERROR_CORRECTION 5  // Tolerância para colisão com plataforma
#define MAX_BLOCKING_ZONES 4

#endif
