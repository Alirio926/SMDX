#ifndef _INC_GAMESTATEMANAGAR_H_
#define _INC_GAMESTATEMANAGAR_H_

#include <genesis.h>

typedef enum {INTRO = 0, TITLE = 1, DEMO = 2, MENU = 3, LEVEL1 = 4, LEVEL1A = 5}GameState;
// Declaração da variável global para ser usada em outros arquivos .c
extern GameState gameState;
// Prototipos
typedef void (*func)(void);
typedef void (*joyHandle)(u16 joy, u16 changed, u16 state);

// Ponto de função para os states
typedef struct _StateManager{
    int result;
    func init;
    func update;
    joyHandle handlerInput;
    func unloadState;
}StateManager;

// prototipos default
void initStateManager(void);
void updateGameState(void);
void loadState(u8 state);
void setState(GameState state);

#endif //_INC_GAMESTATEMANAGAR_H_