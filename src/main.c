// base SGDK include
#include <genesis.h>

#include "main.h"
#include "gamestate_manager.h"

int main(bool hardReset)
{
    // initialization
    VDP_setScreenWidth320();
    // init sprite engine with default parameters
    SPR_init();
    //
    initStateManager();
    //  Start !!!!
    while (TRUE)
    {
        updateGameState();
    }

    return 0;
}

