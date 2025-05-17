/**
 *  \file engine.c
 *  \brief 
 *  \author Alirio Oliveira
 *  \date 04/2025
 *
 * This unit contains Engine
 */
#ifndef ENGINE_H
#define ENGINE_H

#include <genesis.h>
#include <types.h>
#include "xtypes.h"
#include "components/rigidbody.h"
#include "components/entity.h"
#include "physics/physic.h"
#include "entities/player.h"
#include "tiled/tiled_map.h"
#include "core/camera.h"

#define FIX16_TO_S32(f) ((s32)(f) << 10)

// Converte posição fixa (<<6) para inteiro (pixel)
#define POS_TO_INT(x)   ((x) >> 6)
// Converte pixel para posição fixa (<<6)
#define INT_TO_POS(x)   ((x) << 6)
#define SWAP(a, b) { s16 tmp = a; a = b; b = tmp; }

#define checkButton(cJoy, pJoy, button) ((cJoy & button) && !(pJoy & button));

typedef struct {
	u16 joy;
	u16 changed;
	u16 state;
} InputType;

extern InputType input;


#endif 