#include "game_config.h"
#include <maths.h>
#include "core/anim_controller.h"

// Já inicializa com valores padrão diretamente
EntityPhysicsParams defaultPhysicsParams = {
    .gravity       = FIX16(0.50),
    .acceleration  = FIX16(0.25),
    .deceleration  = FIX16(0.20),
    .maxFallSpeed  = FIX16(9.0),
    .jumpSpeed     = FIX16(7.0),
    .maxRunSpeed   = FIX16(3.5)
};

const AnimStateSet defaultAnimSet = {
    .idle   = 0,
    .walk   = 1,
    .run    = 1,
    .jump   = 2,
    .fall   = 2,
    .attack = 5,
    .hurt   = 6,
    .climb  = 7
};