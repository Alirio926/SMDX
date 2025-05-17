// dialogue_font.c — Módulo de inicialização e troca de fonte para diálogo
// -----------------------------------------------------------------------------
#include "dialogue.h"
#include <genesis.h>

// -----------------------------------------------------------------------------
// Inicializa o estado do diálogo com fonte padrão
// -----------------------------------------------------------------------------
void dialogue_init(DialogueState *dlg, u16 fontPal, u16 boxBaseTile)
{
    dlg->fontPalette  = fontPal;
    dlg->boxBaseTile = boxBaseTile;

    dlg->cornerToggle   = 0;
    dlg->updateBoxCtr   = 0;
    dlg->bgBackup       = NULL;
    dlg->hasBackup      = FALSE;
    dlg->finished       = TRUE;

}

// -----------------------------------------------------------------------------
// Carrega uma nova fonte para VRAM e aplica ao estado atual
// -----------------------------------------------------------------------------
void dialogue_setFont(DialogueState *dlg, const Image* font, u16 pal)
{
    VDP_loadFont(font->tileset, DMA);
    dlg->fontPalette = pal;
} 
