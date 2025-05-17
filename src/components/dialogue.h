// =============================================================================
// dialogue.h — Interface principal do sistema de diálogos (Engine-SGDK)
// =============================================================================
#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <genesis.h>

// -----------------------------------------------------------------------------
// Tipos principais
// -----------------------------------------------------------------------------

typedef enum
{
    BOX_OPEN   = 0,
    BOX_CLOSE  = 1,
    BOX_TOGGLE = 2
} BoxAnimMode;

typedef struct {
    bool active;
    // Fontes
    u16 fontPalette;
    const Image* font;

    // Tiles da caixa
    u16 boxBaseTile;

    // Área de texto
    u16 txtX, txtY;
    u16 visW, visH;

    // Estado da impressão
    const char *src;
    u16 i;
    u16 curCol, curRow;
    u16 frameCnt;
    bool finished;

    // Estado da caixa
    u8 cornerToggle;
    u8 updateBoxCtr;

    // Backup de fundo
    u16 *bgBackup;
    u16 bgX, bgY, bgW, bgH;
    bool hasBackup;
} DialogueState;

// -----------------------------------------------------------------------------
// Inicialização e fontes
// -----------------------------------------------------------------------------

void dialogue_init(DialogueState *dlg, u16 fontPal, u16 boxBaseTile);
void dialogue_setFont(DialogueState *dlg, const Image* font, u16 pal);

// -----------------------------------------------------------------------------
// Backup / restauração de fundo
// -----------------------------------------------------------------------------

void dialogue_backupBackground(DialogueState *dlg, u16 x, u16 y, u16 w, u16 h);
void dialogue_restoreBackground(DialogueState *dlg);

// -----------------------------------------------------------------------------
// Animações da caixa
// -----------------------------------------------------------------------------

void dialogue_boxOpenCenter(DialogueState *dlg, u16 x, u16 y, u16 w, u16 h, u16 baseTile, u16 pal, u16 delayFrm);
void dialogue_boxCloseCenter(DialogueState *dlg, u16 x, u16 y, u16 w, u16 h, u16 baseTile, u16 pal, u16 delayFrm);
void dialogue_boxToggleCorners(DialogueState *dlg, u16 x, u16 y, u16 w, u16 h, u16 baseTile, u16 pal);

// -----------------------------------------------------------------------------
// Impressão de texto
// -----------------------------------------------------------------------------

void dialogue_beginText(DialogueState *dlg, u16 x, u16 y, u16 w, u16 h, const char *text);
bool dialogue_updateText(DialogueState *dlg);

#endif // DIALOGUE_H
