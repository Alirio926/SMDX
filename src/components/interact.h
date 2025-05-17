#ifndef INTERACT_H
#define INTERACT_H

#include "components/entity_def.h"

// Distância padrão para interação (em pixels fix16)
#define INTERACT_RADIUS FIX16(32)

void tryInteract(Entity* player);
bool isNear(Vect2D_f16 a, Vect2D_f16 b, fix16 range);
Vect2D_f16 getBodyCenter(RigidBody* body);
Vect2D_f16 getBoxCenter(Box* box);
#endif // INTERACT_H
