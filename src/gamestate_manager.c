#include "gamestate_manager.h"
#include "stages/game.h"

typedef struct {
    func init;
    func update;
    joyHandle input;
    func unload;
} StateFuncs;

static const StateFuncs stateTable[] = {
    [INTRO] = { GameInit, GameUpdate, GameHandlerInput, GameUnloadState },    
};

const u8 STATE_TABLE_SIZE = sizeof(stateTable);
const u8 STATE_FUNCS_SIZE = sizeof(StateFuncs);

// Prototipo interno
void setGameStatesPointers(func a, func b, joyHandle c, func d);
void loadState(u8 state);

// Estado atual
static StateManager CurrentGameState;

// Gerenciadores de troca de estado
GameState gameState = INTRO;  // Começa no menu, por exemplo
static u8 currentState = INTRO;  // Inicializa para evitar valores indefinidos
static u8 nextState = INTRO;

// Inicializa o gerenciador de estados
void initStateManager(){
    JOY_init();
    loadState(currentState);
}

// Carrega um estado
void loadState(u8 state){
    if(state >= STATE_TABLE_SIZE / STATE_FUNCS_SIZE) return;

    const StateFuncs* funcs = &stateTable[state];
    setGameStatesPointers(funcs->init, funcs->update, funcs->input, funcs->unload);
}

// Atualiza ponteiros de funções
void setGameStatesPointers(func a, func b, joyHandle c, func d){
    CurrentGameState.init = a;
    CurrentGameState.update = b;
    CurrentGameState.handlerInput = c;
    CurrentGameState.unloadState = d;

    JOY_setEventHandler(CurrentGameState.handlerInput);

    if (CurrentGameState.init) {
        CurrentGameState.init();  // Chama o construtor do novo estado
    }
}

// Função principal de atualização
void updateGameState(){
    // NULL safe
    if (CurrentGameState.update) 
        CurrentGameState.update();
    

    if (nextState != currentState) {
        // NULL safe
        if (CurrentGameState.unloadState) 
            CurrentGameState.unloadState();
        
        currentState = nextState;
        loadState(currentState);
    }
}

// Define o próximo estado a ser carregado
void setState(GameState state){
    if (state != currentState) {
        nextState = state;
    }
}