/**
 * Meta Hunter - GBA Roguelite Game
 * sprites.c - Sprite graphics using libtonc TILE structure
 */

#include <tonc.h>
#include "sprites.h"

// =============================================================================
// OAM BUFFER (global, used by all rendering code)
// =============================================================================
OBJ_ATTR obj_buffer[128];

// =============================================================================
// SPRITE INITIALIZATION
// =============================================================================
void sprites_init(void) {
    // Initialize OAM
    oam_init(obj_buffer, 128);

    // =========================================================================
    // SPRITE PALETTES
    // =========================================================================
    // Palette 0: Player (cyan)
    pal_obj_bank[0][0] = CLR_BLACK;          // Transparent
    pal_obj_bank[0][1] = RGB15(0, 28, 31);   // Cyan
    pal_obj_bank[0][2] = CLR_WHITE;          // White border
    pal_obj_bank[0][3] = RGB15(0, 16, 20);   // Dark cyan

    // Palette 1: Enemy (red)
    pal_obj_bank[1][0] = CLR_BLACK;
    pal_obj_bank[1][1] = RGB15(31, 8, 8);    // Red
    pal_obj_bank[1][2] = CLR_WHITE;
    pal_obj_bank[1][3] = RGB15(20, 0, 0);    // Dark red

    // Palette 2: Hit flash (white)
    pal_obj_bank[2][0] = CLR_BLACK;
    pal_obj_bank[2][1] = CLR_WHITE;
    pal_obj_bank[2][2] = CLR_WHITE;
    pal_obj_bank[2][3] = CLR_WHITE;

    // =========================================================================
    // CREATE SPRITE TILES
    // =========================================================================

    // --- PLAYER (tiles 0-3): Cyan square with white border ---
    TILE player_tl, player_tr, player_bl, player_br;

    // Top-left: border on top and left
    player_tl.data[0] = 0x22222222;  // Top border row
    player_tl.data[1] = 0x21111112;
    player_tl.data[2] = 0x21111112;
    player_tl.data[3] = 0x21111112;
    player_tl.data[4] = 0x21111112;
    player_tl.data[5] = 0x21111112;
    player_tl.data[6] = 0x21111112;
    player_tl.data[7] = 0x21111112;

    // Top-right: border on top and right
    player_tr.data[0] = 0x22222222;
    player_tr.data[1] = 0x21111112;
    player_tr.data[2] = 0x21111112;
    player_tr.data[3] = 0x21111112;
    player_tr.data[4] = 0x21111112;
    player_tr.data[5] = 0x21111112;
    player_tr.data[6] = 0x21111112;
    player_tr.data[7] = 0x21111112;

    // Bottom-left: border on bottom and left
    player_bl.data[0] = 0x21111112;
    player_bl.data[1] = 0x21111112;
    player_bl.data[2] = 0x21111112;
    player_bl.data[3] = 0x21111112;
    player_bl.data[4] = 0x21111112;
    player_bl.data[5] = 0x21111112;
    player_bl.data[6] = 0x21111112;
    player_bl.data[7] = 0x22222222;  // Bottom border row

    // Bottom-right: border on bottom and right
    player_br.data[0] = 0x21111112;
    player_br.data[1] = 0x21111112;
    player_br.data[2] = 0x21111112;
    player_br.data[3] = 0x21111112;
    player_br.data[4] = 0x21111112;
    player_br.data[5] = 0x21111112;
    player_br.data[6] = 0x21111112;
    player_br.data[7] = 0x22222222;

    tile_mem_obj[0][SPRITE_PLAYER_BASE + 0] = player_tl;
    tile_mem_obj[0][SPRITE_PLAYER_BASE + 1] = player_tr;
    tile_mem_obj[0][SPRITE_PLAYER_BASE + 2] = player_bl;
    tile_mem_obj[0][SPRITE_PLAYER_BASE + 3] = player_br;

    // --- ENEMY WORM (tiles 16-19): Red square with white border ---
    // Reuse same pattern, will use different palette
    tile_mem_obj[0][SPRITE_ENEMY_WORM + 0] = player_tl;
    tile_mem_obj[0][SPRITE_ENEMY_WORM + 1] = player_tr;
    tile_mem_obj[0][SPRITE_ENEMY_WORM + 2] = player_bl;
    tile_mem_obj[0][SPRITE_ENEMY_WORM + 3] = player_br;

    // --- ENEMY TROJAN (tiles 24-27): Same pattern, larger feel from palette ---
    tile_mem_obj[0][SPRITE_ENEMY_TROJAN + 0] = player_tl;
    tile_mem_obj[0][SPRITE_ENEMY_TROJAN + 1] = player_tr;
    tile_mem_obj[0][SPRITE_ENEMY_TROJAN + 2] = player_bl;
    tile_mem_obj[0][SPRITE_ENEMY_TROJAN + 3] = player_br;

    // --- ENEMY TURRET (tiles 32-35) ---
    tile_mem_obj[0][SPRITE_ENEMY_TURRET + 0] = player_tl;
    tile_mem_obj[0][SPRITE_ENEMY_TURRET + 1] = player_tr;
    tile_mem_obj[0][SPRITE_ENEMY_TURRET + 2] = player_bl;
    tile_mem_obj[0][SPRITE_ENEMY_TURRET + 3] = player_br;

    // --- PROJECTILE (tile 40): Small 8x8 filled circle ---
    TILE proj_tile;
    proj_tile.data[0] = 0x00111100;
    proj_tile.data[1] = 0x01111110;
    proj_tile.data[2] = 0x11111111;
    proj_tile.data[3] = 0x11111111;
    proj_tile.data[4] = 0x11111111;
    proj_tile.data[5] = 0x11111111;
    proj_tile.data[6] = 0x01111110;
    proj_tile.data[7] = 0x00111100;

    tile_mem_obj[0][SPRITE_PROJECTILE_PLAYER] = proj_tile;
    tile_mem_obj[0][SPRITE_PROJECTILE_ENEMY] = proj_tile;
}
