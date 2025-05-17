// dialogue_text.c — Impressão e rolagem de texto (Engine-SGDK)
// -----------------------------------------------------------------------------
#include "dialogue.h"
#include <genesis.h>
#include "core/logger.h"

const u32 dialog_box_tiles[9][8] = {
    // Tile 0: canto superior esquerdo
    { 0x00000000, 0x01111110, 0x01222210, 0x01222210, 0x01222210, 0x01222210, 0x01111110, 0x00000000 },
    // Tile 1: borda superior
    { 0x00000000, 0x11111111, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    // Tile 2: canto superior direito
    { 0x00000000, 0x01111110, 0x00002210, 0x00000210, 0x00000210, 0x00000010, 0x00000010, 0x00000000 },
    // Tile 3: borda esquerda
    { 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000, 0x01000000 },
    // Tile 4: centro (vazio ou preenchido)
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    // Tile 5: borda direita
    { 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000010, 0x00000010 },
    // Tile 6: canto inferior esquerdo
    { 0x00000000, 0x01000000, 0x01000000, 0x01200000, 0x01200000, 0x01220000, 0x01111110, 0x00000000 },
    // Tile 7: borda inferior
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x11111111, 0x00000000 },
    // Tile 8: canto inferior direito
    { 0x00000000, 0x01111110, 0x01222210, 0x01222210, 0x01222210, 0x01222210, 0x01111110, 0x00000000 },
};

#define TXT_SPEED      4    // caracteres por quadro

static void scrollUp(DialogueState *dlg)
{
    for (u16 ty = 1; ty < dlg->visH; ++ty)
        for (u16 tx = 0; tx < dlg->visW; ++tx)
        {
            u16 addr = VDP_getPlaneAddress(BG_B, dlg->txtX + tx, dlg->txtY + ty);
            vu16 *ctrl = (vu16 *)VDP_CTRL_PORT;
            vu16 *data = (vu16 *)VDP_DATA_PORT;
            *ctrl = (addr & 0x3FFF);
            *ctrl = 0x0000 | (addr >> 14);
            u16 t = *data;
            VDP_setTileMapXY(BG_B, t, dlg->txtX + tx, dlg->txtY + ty - 1);
        }

    const u16 blank = TILE_ATTR_FULL(dlg->fontPalette, 0, 0, 0, TILE_FONT_INDEX + (' ' - 32));
    for (u16 tx = 0; tx < dlg->visW; ++tx)
        VDP_setTileMapXY(BG_B, blank, dlg->txtX + tx, dlg->txtY + dlg->visH - 1);
}

void dialogue_beginText(DialogueState *dlg, u16 x, u16 y, u16 w, u16 h, const char *text)
{
    if (dlg->active) return;

    dlg->active = TRUE; 
    
    dlg->txtX = x + 1;
    dlg->txtY = y + 1;
    dlg->visW = w - 2;
    dlg->visH = h - 2;
    
    dlg->bgX = x;
    dlg->bgY = y;
    dlg->bgW = w;
    dlg->bgH = h;
    //dialogue_backupBackground(dlg, x, y, w, h);

    VDP_clearTileMapRect(BG_B, dlg->txtX, dlg->txtY, dlg->visW, dlg->visH);

    dlg->i = 0;
    dlg->src = text;
    dlg->curCol = dlg->curRow = 0;
    dlg->frameCnt = 0;
    dlg->finished = FALSE;

    VDP_loadTileData(&dialog_box_tiles[0][0], dlg->boxBaseTile, 9, DMA);

    dialogue_boxOpenCenter(dlg, x, y, w, h, dlg->boxBaseTile, dlg->fontPalette, 2);
}

bool dialogue_updateText(DialogueState *dlg)
{
    if (dlg->finished) return TRUE;

    if (++dlg->frameCnt < TXT_SPEED) return FALSE;
    dlg->frameCnt = 0;

    if (++dlg->updateBoxCtr == 4)
    {
        dialogue_boxToggleCorners(dlg, dlg->txtX - 1, dlg->txtY - 1,
                                  dlg->visW + 2, dlg->visH + 2,
                                  dlg->boxBaseTile, dlg->fontPalette);
        dlg->updateBoxCtr = 0;
    }

    char c = *(dlg->src + dlg->i);
    if (!c) { dlg->finished = TRUE; return TRUE; }

    if (c == '\n' || dlg->curCol >= dlg->visW)
    {
        dlg->curCol = 0;
        dlg->curRow++;
        if (dlg->curRow >= dlg->visH) { scrollUp(dlg); dlg->curRow = dlg->visH - 1; }
        if (c == '\n') { dlg->i++; return FALSE; }
    }

    u16 tile = TILE_FONT_INDEX + (c - 32);
    VDP_setTileMapXY(BG_B,
        TILE_ATTR_FULL(dlg->fontPalette, 1, 0, 0, tile),
        dlg->txtX + dlg->curCol, dlg->txtY + dlg->curRow);

    dlg->curCol++;
    dlg->i++;
    return FALSE;
}
