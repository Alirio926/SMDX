#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "components/dialogue.h"
#include "core/game_config.h"

extern DialogueState activeDialogue;
extern UpdateState g_gameState;

void GameInit();
void GameUpdate(void);
void GameHandlerInput(u16 joy, u16 changed, u16 state);
void GameUnloadState();

#endif