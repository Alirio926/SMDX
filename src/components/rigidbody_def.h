#ifndef RIGIDBODY_DEF_H
#define RIGIDBODY_DEF_H

#include "types.h"
#include "xtypes.h"
#include "core/game_config.h"
#include "core/update_policy.h"

typedef struct RigidBody
{
    fix16 acceleration;
    fix16 deceleration;
    Vect2D_s16 position; // Posição atual
    Vect2D_s16 globalPosition;
    Vect2D_u16 centerOffset;
    AABB playerBounds;// Caixa de colisão (offset + size)
    AABB aabb; // Caixa de colisão (size)

    struct
    {
        s32 fixX;
        s16 x;
        s32 fixY;
    } velocity;       // Velocidade atual
    Vect2D_f16 delta; // Movimento do frame atual (pos - pos anterior)

    u8 layer; // A que grupo pertence
    u8 mask;  // Com quais grupos pode colidir
    u16 tag;  // Tipo específico de entidade (opcional)

    void *owner;                                                          // Ponteiro para entidade dona
    void (*onCollision)(struct RigidBody *self, struct RigidBody *other); // Callback física
    void (*onTrigger)(struct RigidBody *self, struct RigidBody *other);   // Callback triggers

    EntityPhysicsParams *physics; // Referência à física base
    struct RigidBody *support;    // Corpo abaixo (ex: plataforma)

    VerticalState vState;
    MovementState mState;
    ActionState aState;
    
    bool collidable; // Responde a colisões físicas
    bool active;     // Deve ser atualizado
    u16 index;       // Índice no array interno
    
    UpdatePolicy physicsPolicy;
} RigidBody;

#endif // RIGIDBODY_DEF_H
