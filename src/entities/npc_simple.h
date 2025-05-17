// npc_simple.h — Interface para NPCs estáticos com falas múltiplas
// -----------------------------------------------------------------------------
#ifndef NPC_SIMPLE_H
#define NPC_SIMPLE_H

#include <genesis.h>
#include "components/dialogue.h"
#include "components/entity.h"

#define MAX_NPC_TEXTS 4

/*
Modo	    Comportamento
LOOP	    Repete ciclicamente as falas
STOP_LAST	Para na última, e repete sempre ela
ADVANCE	    Avança e nunca mais repete (ex: tutorial, cutscene)
*/
typedef enum {
    NPC_TEXTMODE_LOOP,      // volta ao início quando termina
    NPC_TEXTMODE_STOP_LAST, // para na última fala
    NPC_TEXTMODE_ADVANCE    // avança para a próxima, e depois... nada
} NpcTextMode;

typedef struct {
    Box hitbox;                      // área de interação
    const char* texts[MAX_NPC_TEXTS]; // lista de falas
    u16 textCount;                    // número total de falas
    u16 textIndex;                    // índice atual
    NpcTextMode textMode;
} NpcSimpleDef;

Entity* npc_createSimple(const NpcSimpleDef* def);
void npc_simple_onInteract(Entity* e);

#endif // NPC_SIMPLE_H
