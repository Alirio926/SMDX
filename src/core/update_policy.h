#ifndef _UPDATE_POLICY_H_
#define _UPDATE_POLICY_H_

#include "xtypes.h"

typedef enum {
    UPDATE_ALWAYS = 0,         // Atualiza sempre
    UPDATE_VISIBLE_ONLY,       // Atualiza se visível na tela
    UPDATE_NEAR_CAMERA,        // Atualiza se dentro de margem estendida
    UPDATE_DISABLED            // Nunca atualiza
} UpdatePolicy;

bool should_update(Vect2D_s16* globalPosition, AABB* aabb, u8 mode);
#endif
