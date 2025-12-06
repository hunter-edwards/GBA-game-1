/**
 * Meta Hunter - GBA Roguelite Game
 * sprites.c - Sprite initialization with simple debug graphics
 */

#include <tonc.h>
#include "sprites.h"

// =============================================================================
// OAM BUFFER
// =============================================================================
OBJ_ATTR obj_buffer[128];

// =============================================================================
// HELPER: Fill an 8x8 4bpp tile with a solid color
// =============================================================================
static void fill_tile(u32* tile, u8 color) {
    // In 4bpp, each pixel is 4 bits, so 8 pixels = 32 bits = 1 u32 per row
    // To fill with color 'c', we need: c | (c<<4) | (c<<8) | ... for 8 pixels
    u32 row = color | (color << 4) | (color << 8) | (color << 12) |
              (color << 16) | (color << 20) | (color << 24) | (color << 28);
    for (int i = 0; i < 8; i++) {
        tile[i] = row;
    }
}

// =============================================================================
// HELPER: Create a simple bordered square sprite (16x16 = 4 tiles)
// =============================================================================
static void create_box_sprite(int tile_index, u8 fill_color, u8 border_color) {
    // Get pointer to the 4 tiles (each 8x8 tile is 8 u32s in 4bpp)
    u32* tiles = (u32*)&tile_mem_obj[0][tile_index];

    // Create the fill and border row patterns
    u32 fill_row = fill_color | (fill_color << 4) | (fill_color << 8) | (fill_color << 12) |
                   (fill_color << 16) | (fill_color << 20) | (fill_color << 24) | (fill_color << 28);
    u32 border_row = border_color | (border_color << 4) | (border_color << 8) | (border_color << 12) |
                     (border_color << 16) | (border_color << 20) | (border_color << 24) | (border_color << 28);

    // Mixed row: border on edges, fill in middle
    // For an 8-pixel row: B B F F F F B B
    u32 mixed_row = border_color | (border_color << 4) |
                    (fill_color << 8) | (fill_color << 12) | (fill_color << 16) | (fill_color << 20) |
                    (border_color << 24) | (border_color << 28);

    // Top-left tile (0): top-left corner
    tiles[0] = border_row;  // Top edge
    tiles[1] = border_row;
    tiles[2] = mixed_row;
    tiles[3] = mixed_row;
    tiles[4] = mixed_row;
    tiles[5] = mixed_row;
    tiles[6] = mixed_row;
    tiles[7] = mixed_row;

    // Top-right tile (1): top-right corner
    tiles[8] = border_row;
    tiles[9] = border_row;
    tiles[10] = mixed_row;
    tiles[11] = mixed_row;
    tiles[12] = mixed_row;
    tiles[13] = mixed_row;
    tiles[14] = mixed_row;
    tiles[15] = mixed_row;

    // Bottom-left tile (2): bottom-left corner
    tiles[16] = mixed_row;
    tiles[17] = mixed_row;
    tiles[18] = mixed_row;
    tiles[19] = mixed_row;
    tiles[20] = mixed_row;
    tiles[21] = mixed_row;
    tiles[22] = border_row;
    tiles[23] = border_row;

    // Bottom-right tile (3): bottom-right corner
    tiles[24] = mixed_row;
    tiles[25] = mixed_row;
    tiles[26] = mixed_row;
    tiles[27] = mixed_row;
    tiles[28] = mixed_row;
    tiles[29] = mixed_row;
    tiles[30] = border_row;
    tiles[31] = border_row;
}

// =============================================================================
// HELPER: Create a small 8x8 filled circle for projectiles
// =============================================================================
static void create_circle_tile(int tile_index, u8 color) {
    u32* tile = (u32*)&tile_mem_obj[0][tile_index];

    // Simple circle pattern (0 = transparent, color = filled)
    // Row 0: ..XX..
    tile[0] = (color << 8) | (color << 12) | (color << 16) | (color << 20);
    // Row 1: .XXXX.
    tile[1] = (color << 4) | (color << 8) | (color << 12) | (color << 16) | (color << 20) | (color << 24);
    // Row 2-5: XXXXXX
    u32 full = color | (color << 4) | (color << 8) | (color << 12) | (color << 16) | (color << 20) | (color << 24) | (color << 28);
    tile[2] = full;
    tile[3] = full;
    tile[4] = full;
    tile[5] = full;
    // Row 6: .XXXX.
    tile[6] = (color << 4) | (color << 8) | (color << 12) | (color << 16) | (color << 20) | (color << 24);
    // Row 7: ..XX..
    tile[7] = (color << 8) | (color << 12) | (color << 16) | (color << 20);
}

// =============================================================================
// SPRITE INITIALIZATION
// =============================================================================
void sprites_init(void) {
    // Initialize OAM buffer (all sprites hidden)
    oam_init(obj_buffer, 128);

    // =========================================================================
    // SET UP SPRITE PALETTES
    // =========================================================================
    // Palette 0: Player (bright cyan on black)
    pal_obj_mem[0] = RGB15(0, 0, 0);        // Transparent (black)
    pal_obj_mem[1] = RGB15(0, 31, 31);      // Bright cyan (main)
    pal_obj_mem[2] = RGB15(31, 31, 31);     // White (border/highlight)
    pal_obj_mem[3] = RGB15(0, 16, 16);      // Dark cyan

    // Palette 1: Enemy (bright red)
    pal_obj_mem[16 + 0] = RGB15(0, 0, 0);   // Transparent
    pal_obj_mem[16 + 1] = RGB15(31, 8, 8);  // Bright red (main)
    pal_obj_mem[16 + 2] = RGB15(31, 31, 31);// White (border)
    pal_obj_mem[16 + 3] = RGB15(16, 0, 0);  // Dark red

    // Palette 2: White flash
    pal_obj_mem[32 + 0] = RGB15(0, 0, 0);
    pal_obj_mem[32 + 1] = RGB15(31, 31, 31);
    pal_obj_mem[32 + 2] = RGB15(31, 31, 31);
    pal_obj_mem[32 + 3] = RGB15(31, 31, 31);

    // =========================================================================
    // CREATE SPRITE TILES
    // =========================================================================

    // Player sprite: cyan box with white border (tiles 0-3)
    create_box_sprite(SPRITE_PLAYER_BASE, 1, 2);  // fill=cyan(1), border=white(2)

    // Enemy Worm: red box with white border (tiles 16-19)
    create_box_sprite(SPRITE_ENEMY_WORM, 1, 2);

    // Enemy Trojan: red box (tiles 24-27)
    create_box_sprite(SPRITE_ENEMY_TROJAN, 1, 2);

    // Enemy Turret: red box (tiles 32-35) - same for now
    create_box_sprite(SPRITE_ENEMY_TURRET, 1, 2);

    // Player projectile: small cyan circle (tile 40)
    create_circle_tile(SPRITE_PROJECTILE_PLAYER, 1);

    // Enemy projectile: small red circle (tile 41)
    create_circle_tile(SPRITE_PROJECTILE_ENEMY, 1);
}
