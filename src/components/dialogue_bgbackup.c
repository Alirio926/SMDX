// dialogue_bgbackup.c — Backup/restauração de fundo (BG_B)
// -----------------------------------------------------------------------------
#include "dialogue.h"
#include <genesis.h>

static u16 readVRAM_u16(u16 addr)
{
    vu16 *ctrl = (vu16 *)VDP_CTRL_PORT;
    vu16 *data = (vu16 *)VDP_DATA_PORT;

    *ctrl = (addr & 0x3FFF);
    *ctrl = 0x0000 | (addr >> 14);
    return *data;
}

static u16 getTileXY(u16 x, u16 y)
{
    u16 addr = VDP_getPlaneAddress(BG_B, x, y);
    return readVRAM_u16(addr);
}

void dialogue_backupBackground(DialogueState *dlg, u16 x, u16 y, u16 w, u16 h)
{
    if (dlg->bgBackup)
    {
        free(dlg->bgBackup);
        dlg->bgBackup = NULL;
    }

    dlg->bgBackup = malloc(w * h * sizeof(u16));
    if (!dlg->bgBackup) return;

    dlg->bgX = x;
    dlg->bgY = y;
    dlg->bgW = w;
    dlg->bgH = h;

    for (u16 ty = 0; ty < h; ++ty)
        for (u16 tx = 0; tx < w; ++tx)
            dlg->bgBackup[ty * w + tx] = getTileXY(x + tx, y + ty);

    dlg->hasBackup = TRUE;
}

void dialogue_restoreBackground(DialogueState *dlg)
{
    if (!dlg->hasBackup || !dlg->bgBackup) return;

    for (u16 ty = 0; ty < dlg->bgH; ++ty)
        for (u16 tx = 0; tx < dlg->bgW; ++tx)
            VDP_setTileMapXY(BG_B,
                dlg->bgBackup[ty * dlg->bgW + tx],
                dlg->bgX + tx, dlg->bgY + ty);

    free(dlg->bgBackup);
    dlg->bgBackup = NULL;
    dlg->hasBackup = FALSE;
} 
