
#ifndef PATH_DEF_H
#define PATH_DEF_H

#include "xtypes.h"

typedef enum {
    DIST_EUCLIDEAN = 0,  // Distância real: sqrt(x² + y²)
    DIST_MANHATTAN = 1   // Distância em blocos: abs(x) + abs(y)
} DistanceType;

typedef enum {
    PATHMODE_LOOP,
    PATHMODE_PINGPONG,
    PATHMODE_ONESHOT,
    PATHMODE_INVALID
} PATHMODE;

/**
 * Obj extraido do tiled
 */
typedef struct {
    Vect2D_u16 position;
    const Vect2D_u16* path;
    u8 path_len;
    u8 delayInicial;
    u8 path_mode;           // PATH_MODE_LOOP, etc.
    fix16 velocidade;       // em FIX16
    const char* sprite;     // nome do sprite (opcional)
    u8 w, h;                // dimensões (se tiver sprite)
} PathAgentDef;

#endif