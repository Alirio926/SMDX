// dialogue_box.c — Animações da caixa de diálogo (Engine-SGDK)
// -----------------------------------------------------------------------------
#include "dialogue.h"
#include <genesis.h>

// Estrutura auxiliar para cantos
typedef struct { u8 tile, hf, vf; } CornerInfo;

// Mapeamento de cantos (toggle 0 / 1)
static const CornerInfo s_cornerMap[2][4] = {
    { {0,0,0}, {2,0,0}, {6,0,0}, {8,0,0} },  // normal
    { {6,1,0}, {8,1,1}, {0,1,1}, {2,1,0} }   // invertido
};

static void putTile(u16 x, u16 y, u16 base, u16 pal, u8 tile, u8 hf, u8 vf) {
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(pal, 1, hf, vf, base + tile), x, y);
}

static void fillRect(u16 x, u16 y, u16 w, u16 h, u16 base, u16 pal) {
    for (u16 ty = 0; ty < h; ty++)
        for (u16 tx = 0; tx < w; tx++)
            putTile(x + tx, y + ty, base, pal, 4, 0, 0);
}

static void drawCorners(u16 x, u16 y, u16 w, u16 h, u16 base, u16 pal, u8 toggle) {
    u16 rx = x + w - 1;
    u16 by = y + h - 1;

    const CornerInfo *ci = &s_cornerMap[toggle][0];
    putTile(x,  y,  base, pal, ci->tile, ci->hf, ci->vf);
    ci = &s_cornerMap[toggle][1];
    putTile(rx, y,  base, pal, ci->tile, ci->hf, ci->vf);
    ci = &s_cornerMap[toggle][2];
    putTile(x,  by, base, pal, ci->tile, ci->hf, ci->vf);
    ci = &s_cornerMap[toggle][3];
    putTile(rx, by, base, pal, ci->tile, ci->hf, ci->vf);
}

static void drawBox(u16 x, u16 y, u16 w, u16 h, u16 base, u16 pal, u8 cornerTog) {
    for (u16 tx = 1; tx < w - 1; tx++) {
        putTile(x + tx, y,         base, pal, 1, 0, 0);
        putTile(x + tx, y + h - 1, base, pal, 7, 0, 0);
    }
    for (u16 ty = 1; ty < h - 1; ty++) {
        putTile(x,         y + ty, base, pal, 3, 0, 0);
        putTile(x + w - 1, y + ty, base, pal, 5, 0, 0);
    }
    drawCorners(x, y, w, h, base, pal, cornerTog);
}

void dialogue_boxOpenCenter(DialogueState *dlg, u16 x, u16 y, u16 w, u16 h, u16 base, u16 pal, u16 delay) {
    const u16 cx = x + w / 2;
    const u16 cy = y + h / 2;
    for (u16 s = 1; s <= ((w > h) ? w : h); s += 2) {
        u16 cw = (s > w) ? w : s;
        u16 ch = (s > h) ? h : s;
        u16 dx = cx - cw / 2;
        u16 dy = cy - ch / 2;

        fillRect(x, y, w, h, base, pal);
        drawBox(dx, dy, cw, ch, base, pal, dlg->cornerToggle);

        for (u16 d = 0; d < delay; ++d) SYS_doVBlankProcess();
    }
}

void dialogue_boxCloseCenter(DialogueState *dlg, u16 x, u16 y, u16 w, u16 h, u16 base, u16 pal, u16 delay) {
    const u16 cx = x + w / 2;
    const u16 cy = y + h / 2;
    for (s16 s = ((w > h) ? w : h); s >= 1; s -= 2) {
        fillRect(x, y, w, h, base, pal);
        u16 cw = (s > w) ? w : s;
        u16 ch = (s > h) ? h : s;
        u16 dx = cx - cw / 2;
        u16 dy = cy - ch / 2;

        drawBox(dx, dy, cw, ch, base, pal, dlg->cornerToggle);

        dlg->active = FALSE;
        
        for (u16 d = 0; d < delay; ++d) SYS_doVBlankProcess();
    }
    fillRect(x, y, w, h, base, pal);
    //dialogue_restoreBackground(dlg);
}

void dialogue_boxToggleCorners(DialogueState *dlg, u16 x, u16 y, u16 w, u16 h, u16 base, u16 pal) {
    dlg->cornerToggle ^= 1;
    drawCorners(x, y, w, h, base, pal, dlg->cornerToggle);
} 
