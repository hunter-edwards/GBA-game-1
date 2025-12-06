/**
 * Meta Hunter - GBA Roguelite Game
 * sprites.c - Sprite initialization and placeholder graphics
 *
 * This creates simple procedural placeholder sprites.
 * Replace with actual sprite graphics from .png files using grit.
 */

#include <tonc.h>
#include "sprites.h"

// =============================================================================
// OAM BUFFER
// =============================================================================
OBJ_ATTR obj_buffer[128];

// =============================================================================
// PLACEHOLDER TILE DATA
// =============================================================================
// 4bpp tiles: each byte contains 2 pixels (4 bits each)
// 8x8 tile = 32 bytes

// Simple 8x8 tile patterns
static void create_tile_8x8(u32* dest, u8 pattern[8]) {
    // Convert 1-bit pattern to 4bpp tile
    for (int row = 0; row < 8; row++) {
        u32 row_data = 0;
        for (int col = 0; col < 8; col++) {
            u8 pixel = (pattern[row] & (0x80 >> col)) ? 1 : 0;
            row_data |= (pixel << (col * 4));
        }
        dest[row] = row_data;
    }
}

// Create a 16x16 sprite from 4 8x8 tiles
static void create_sprite_16x16(int tile_index, u8 pattern[16][16], u8 color) {
    u32* tile_base = (u32*)&tile_mem[4][tile_index * 8];

    // Process each 8x8 quadrant
    for (int qy = 0; qy < 2; qy++) {
        for (int qx = 0; qx < 2; qx++) {
            u32* tile = tile_base + (qy * 2 + qx) * 8;

            for (int row = 0; row < 8; row++) {
                u32 row_data = 0;
                for (int col = 0; col < 8; col++) {
                    int px = qx * 8 + col;
                    int py = qy * 8 + row;
                    u8 pixel = pattern[py][px] ? color : 0;
                    row_data |= (pixel << (col * 4));
                }
                tile[row] = row_data;
            }
        }
    }
}

// =============================================================================
// PLACEHOLDER SPRITE PATTERNS
// =============================================================================

// Player sprite pattern (16x16 humanoid shape)
static u8 player_pattern[16][16] = {
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,1,1,0,0,1,1,1,1,1,1,0,0,1,1,0},
    {0,1,1,0,0,1,1,1,1,1,1,0,0,1,1,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},
    {0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0},
    {0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0},
};

// Worm enemy (wiggly shape)
static u8 worm_pattern[16][16] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0},
    {0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0},
    {0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0},
    {0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0},
    {0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0},
    {0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0},
    {0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0},
    {0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0},
    {0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

// Trojan enemy (bulky shape)
static u8 trojan_pattern[16][16] = {
    {0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0},
    {0,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0},
    {0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0},
    {0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0},
    {0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0},
    {0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0},
};

// Turret enemy (square with barrel)
static u8 turret_pattern[16][16] = {
    {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,1,1,0,0,1,1,1,1,0,0,1,1,0,0},
    {0,0,1,1,0,0,1,1,1,1,0,0,1,1,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,1,1,0,0,1,1,1,1,0,0,1,1,0,0},
    {0,0,1,1,0,0,1,1,1,1,0,0,1,1,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

// Projectile (8x8 small dot)
static u8 projectile_pattern[8] = {
    0b00111100,
    0b01111110,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b01111110,
    0b00111100,
};

// =============================================================================
// SPRITE INITIALIZATION
// =============================================================================
void sprites_init(void) {
    // Initialize OAM buffer
    oam_init(obj_buffer, 128);

    // =========================================================================
    // SET UP PALETTES
    // =========================================================================
    // Palette 0: Player (cyan/blue)
    pal_obj_mem[0] = RGB15(0, 0, 0);      // Transparent
    pal_obj_mem[1] = RGB15(8, 24, 28);    // Main color (cyan)
    pal_obj_mem[2] = RGB15(4, 16, 20);    // Dark shade
    pal_obj_mem[3] = RGB15(12, 28, 31);   // Highlight

    // Palette 1: Enemy (red/orange)
    pal_obj_mem[16] = RGB15(0, 0, 0);     // Transparent
    pal_obj_mem[17] = RGB15(28, 8, 8);    // Main color (red)
    pal_obj_mem[18] = RGB15(20, 4, 4);    // Dark shade
    pal_obj_mem[19] = RGB15(31, 16, 8);   // Highlight (orange)

    // Palette 2: Flash/hit (white)
    pal_obj_mem[32] = RGB15(0, 0, 0);     // Transparent
    pal_obj_mem[33] = RGB15(31, 31, 31);  // White
    pal_obj_mem[34] = RGB15(28, 28, 28);  // Light gray
    pal_obj_mem[35] = RGB15(31, 31, 31);  // White

    // =========================================================================
    // CREATE PLACEHOLDER SPRITES
    // =========================================================================

    // Player sprites
    create_sprite_16x16(SPRITE_PLAYER_BASE, player_pattern, 1);
    create_sprite_16x16(SPRITE_PLAYER_FRAME2, player_pattern, 1);  // Same for now
    create_sprite_16x16(SPRITE_PLAYER_UP, player_pattern, 1);
    create_sprite_16x16(SPRITE_PLAYER_DOWN, player_pattern, 1);

    // Enemy sprites
    create_sprite_16x16(SPRITE_ENEMY_WORM, worm_pattern, 1);
    create_sprite_16x16(SPRITE_ENEMY_WORM + 4, worm_pattern, 1);  // Anim frame 2
    create_sprite_16x16(SPRITE_ENEMY_TROJAN, trojan_pattern, 1);
    create_sprite_16x16(SPRITE_ENEMY_TROJAN + 4, trojan_pattern, 1);
    create_sprite_16x16(SPRITE_ENEMY_TURRET, turret_pattern, 1);
    create_sprite_16x16(SPRITE_ENEMY_TURRET + 4, turret_pattern, 1);

    // Projectile sprites (8x8)
    u32* proj_tile = (u32*)&tile_mem[4][SPRITE_PROJECTILE_PLAYER * 8];
    create_tile_8x8(proj_tile, projectile_pattern);

    proj_tile = (u32*)&tile_mem[4][SPRITE_PROJECTILE_ENEMY * 8];
    create_tile_8x8(proj_tile, projectile_pattern);
}
