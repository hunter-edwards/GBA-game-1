/**
 * Meta Hunter - GBA Roguelite Game
 * sprites.c - Sprite initialization and placeholder graphics
 */

#include <tonc.h>
#include "sprites.h"

// =============================================================================
// OAM BUFFER
// =============================================================================
OBJ_ATTR obj_buffer[128];

// =============================================================================
// SPRITE INITIALIZATION
// =============================================================================
void sprites_init(void) {
    // Initialize OAM buffer (all sprites hidden)
    oam_init(obj_buffer, 128);

    // =========================================================================
    // SET UP SPRITE PALETTES
    // =========================================================================
    // Palette 0: Player (cyan)
    pal_obj_mem[0] = RGB15(0, 0, 0);       // Transparent
    pal_obj_mem[1] = RGB15(0, 20, 28);     // Cyan body
    pal_obj_mem[2] = RGB15(0, 28, 31);     // Light cyan highlight
    pal_obj_mem[3] = RGB15(0, 12, 16);     // Dark cyan shadow

    // Palette 1: Enemy (red)
    pal_obj_mem[16 + 0] = RGB15(0, 0, 0);  // Transparent
    pal_obj_mem[16 + 1] = RGB15(28, 8, 8); // Red body
    pal_obj_mem[16 + 2] = RGB15(31, 16, 8);// Orange highlight
    pal_obj_mem[16 + 3] = RGB15(16, 4, 4); // Dark red shadow

    // Palette 2: White flash (for hit effect)
    pal_obj_mem[32 + 0] = RGB15(0, 0, 0);
    pal_obj_mem[32 + 1] = RGB15(31, 31, 31);
    pal_obj_mem[32 + 2] = RGB15(31, 31, 31);
    pal_obj_mem[32 + 3] = RGB15(28, 28, 28);

    // =========================================================================
    // CREATE SPRITE TILES
    // =========================================================================
    // Sprites use tile_mem[4] (OBJ character block 0)
    // Each 4bpp 8x8 tile is 32 bytes = 8 u32s

    // ----- PLAYER SPRITE (16x16 = 4 tiles) -----
    // Tile indices 0-3 for player
    u32* player_tiles = (u32*)tile_mem_obj[0];

    // Simple humanoid shape
    // Top-left tile (8x8)
    player_tiles[0] = 0x00000000;
    player_tiles[1] = 0x00011100;
    player_tiles[2] = 0x00111110;
    player_tiles[3] = 0x00111110;
    player_tiles[4] = 0x00111110;
    player_tiles[5] = 0x00011100;
    player_tiles[6] = 0x00111110;
    player_tiles[7] = 0x01111111;

    // Top-right tile (8x8)
    player_tiles[8]  = 0x00000000;
    player_tiles[9]  = 0x00111000;
    player_tiles[10] = 0x01111100;
    player_tiles[11] = 0x01111100;
    player_tiles[12] = 0x01111100;
    player_tiles[13] = 0x00111000;
    player_tiles[14] = 0x01111100;
    player_tiles[15] = 0x11111110;

    // Bottom-left tile (8x8)
    player_tiles[16] = 0x01111111;
    player_tiles[17] = 0x00011110;
    player_tiles[18] = 0x00011110;
    player_tiles[19] = 0x00011110;
    player_tiles[20] = 0x00011000;
    player_tiles[21] = 0x00011100;
    player_tiles[22] = 0x00011100;
    player_tiles[23] = 0x00111100;

    // Bottom-right tile (8x8)
    player_tiles[24] = 0x11111110;
    player_tiles[25] = 0x01111000;
    player_tiles[26] = 0x01111000;
    player_tiles[27] = 0x01111000;
    player_tiles[28] = 0x00011000;
    player_tiles[29] = 0x00111000;
    player_tiles[30] = 0x00111000;
    player_tiles[31] = 0x00111100;

    // ----- ENEMY WORM SPRITE (16x16 = 4 tiles, starting at tile 16) -----
    u32* worm_tiles = (u32*)tile_mem_obj[0] + (SPRITE_ENEMY_WORM * 8);

    // Top-left
    worm_tiles[0] = 0x00000000;
    worm_tiles[1] = 0x00001110;
    worm_tiles[2] = 0x00011111;
    worm_tiles[3] = 0x00111111;
    worm_tiles[4] = 0x01111110;
    worm_tiles[5] = 0x01111100;
    worm_tiles[6] = 0x11111000;
    worm_tiles[7] = 0x11110000;

    // Top-right
    worm_tiles[8]  = 0x00000000;
    worm_tiles[9]  = 0x01110000;
    worm_tiles[10] = 0x11111000;
    worm_tiles[11] = 0x11111100;
    worm_tiles[12] = 0x01111110;
    worm_tiles[13] = 0x00111110;
    worm_tiles[14] = 0x00011111;
    worm_tiles[15] = 0x00001111;

    // Bottom-left
    worm_tiles[16] = 0x11110000;
    worm_tiles[17] = 0x11111000;
    worm_tiles[18] = 0x01111100;
    worm_tiles[19] = 0x01111110;
    worm_tiles[20] = 0x00111111;
    worm_tiles[21] = 0x00011111;
    worm_tiles[22] = 0x00001110;
    worm_tiles[23] = 0x00000000;

    // Bottom-right
    worm_tiles[24] = 0x00001111;
    worm_tiles[25] = 0x00011111;
    worm_tiles[26] = 0x00111110;
    worm_tiles[27] = 0x01111110;
    worm_tiles[28] = 0x11111100;
    worm_tiles[29] = 0x11111000;
    worm_tiles[30] = 0x01110000;
    worm_tiles[31] = 0x00000000;

    // ----- ENEMY TROJAN SPRITE (16x16 = 4 tiles, starting at tile 24) -----
    u32* trojan_tiles = (u32*)tile_mem_obj[0] + (SPRITE_ENEMY_TROJAN * 8);

    // Top-left (bulky square shape)
    trojan_tiles[0] = 0x00111111;
    trojan_tiles[1] = 0x01111111;
    trojan_tiles[2] = 0x11111111;
    trojan_tiles[3] = 0x11100111;
    trojan_tiles[4] = 0x11100111;
    trojan_tiles[5] = 0x11111111;
    trojan_tiles[6] = 0x11111111;
    trojan_tiles[7] = 0x11111111;

    // Top-right
    trojan_tiles[8]  = 0x11111100;
    trojan_tiles[9]  = 0x11111110;
    trojan_tiles[10] = 0x11111111;
    trojan_tiles[11] = 0x11100111;
    trojan_tiles[12] = 0x11100111;
    trojan_tiles[13] = 0x11111111;
    trojan_tiles[14] = 0x11111111;
    trojan_tiles[15] = 0x11111111;

    // Bottom-left
    trojan_tiles[16] = 0x11111111;
    trojan_tiles[17] = 0x11111111;
    trojan_tiles[18] = 0x01111111;
    trojan_tiles[19] = 0x01111111;
    trojan_tiles[20] = 0x00111110;
    trojan_tiles[21] = 0x00111110;
    trojan_tiles[22] = 0x00111110;
    trojan_tiles[23] = 0x01111110;

    // Bottom-right
    trojan_tiles[24] = 0x11111111;
    trojan_tiles[25] = 0x11111111;
    trojan_tiles[26] = 0x11111110;
    trojan_tiles[27] = 0x11111110;
    trojan_tiles[28] = 0x01111100;
    trojan_tiles[29] = 0x01111100;
    trojan_tiles[30] = 0x01111100;
    trojan_tiles[31] = 0x01111110;

    // ----- PROJECTILE SPRITE (8x8 = 1 tile, at tile 40) -----
    u32* proj_tiles = (u32*)tile_mem_obj[0] + (SPRITE_PROJECTILE_PLAYER * 8);

    proj_tiles[0] = 0x00011000;
    proj_tiles[1] = 0x00111100;
    proj_tiles[2] = 0x01111110;
    proj_tiles[3] = 0x11111111;
    proj_tiles[4] = 0x11111111;
    proj_tiles[5] = 0x01111110;
    proj_tiles[6] = 0x00111100;
    proj_tiles[7] = 0x00011000;

    // Enemy projectile at tile 41 (same pattern, different palette will be used)
    u32* enemy_proj = (u32*)tile_mem_obj[0] + (SPRITE_PROJECTILE_ENEMY * 8);
    for (int i = 0; i < 8; i++) {
        enemy_proj[i] = proj_tiles[i];
    }
}
