#ifndef PHYSIC_H
#define PHYSIC_H

#include "types.h"
#include "components/rigidbody_def.h"

#define INT_TO_POS(x) ((x) << 6)
#define POS_TO_INT(x) ((x) >> 6)

// Inicializa o sistema de física
void physics_init();

// Atualiza todos os corpos ativos
void physics_updateAll();

// Define a gravidade global
void physics_setGravity(fix16 value);

// Aplica um impulso direto ao corpo (ex: pulo, empurrão, dano)
void physics_applyImpulse(RigidBody* body, fix16 dx, fix16 dy);

// Define o mapa de colisão atual usado pelo sistema de física
void physics_setCollisionMap(u8** map, u16 width, u16 height);

// Verifica colisão entre dois corpos (bounding box)
bool physics_checkCollision(const RigidBody* a, const RigidBody* b);

// Retorna true se o corpo estiver colidindo com o cenário embaixo
bool physics_checkGrounded(const RigidBody* body);

bool aabb_intersect(const AABB* a, const AABB* b);

void rigidbody_updateAABB(RigidBody* rb, u16 width, u16 height);

void rigidbody_getGlobalAABB(RigidBody* body, AABB* out);

bool aabb_intersectBox(const Box* box, const AABB* aabb);

bool box_intersect(const Box* a, const Box* b);
#endif // PHYSIC_H
