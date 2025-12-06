/**
 * Meta Hunter - GBA Roguelite Game
 * main.c - MINIMAL TEST using libtonc properly
 */

#include <tonc.h>
#include "sprites.h"

// obj_buffer is defined in sprites.c, declared extern in sprites.h

int main(void) {
    // Initialize libtonc interrupt system
    irq_init(NULL);
    irq_enable(II_VBLANK);

    // Set display: Mode 0, enable sprites, 1D sprite mapping
    REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;

    // Initialize OAM (hides all sprites)
    oam_init(obj_buffer, 128);

    // =========================================================================
    // SET UP SPRITE PALETTE
    // =========================================================================
    // Use memset16/memset32 for VRAM writes (libtonc safe functions)

    // Palette bank 0 for sprites
    pal_obj_bank[0][0] = CLR_BLACK;           // Color 0: transparent
    pal_obj_bank[0][1] = CLR_MAG;             // Color 1: magenta
    pal_obj_bank[0][2] = CLR_YELLOW;          // Color 2: yellow
    pal_obj_bank[0][3] = CLR_LIME;            // Color 3: lime green

    // =========================================================================
    // CREATE SPRITE TILE DATA
    // =========================================================================
    // For 4bpp sprites, each tile is 32 bytes (8 rows * 4 bytes per row)
    // Each pixel is 4 bits, so 8 pixels = 32 bits = 1 word per row

    // Create a solid color tile using the TILE structure
    // tile_mem_obj[charblock][tile_index]
    // We'll use charblock 0, tiles 0-3 for a 16x16 sprite

    TILE solid_tile;
    // Fill all 8 rows with color index 1 (magenta)
    // 0x11111111 = 8 pixels, each with color index 1
    for (int i = 0; i < 8; i++) {
        solid_tile.data[i] = 0x11111111;
    }

    // Copy tile data to OBJ VRAM (tiles 0, 1, 2, 3)
    tile_mem_obj[0][0] = solid_tile;
    tile_mem_obj[0][1] = solid_tile;
    tile_mem_obj[0][2] = solid_tile;
    tile_mem_obj[0][3] = solid_tile;

    // Also create a different colored tile for testing
    TILE yellow_tile;
    for (int i = 0; i < 8; i++) {
        yellow_tile.data[i] = 0x22222222;  // Color index 2 (yellow)
    }
    tile_mem_obj[0][4] = yellow_tile;

    // =========================================================================
    // SET UP SPRITE IN OAM
    // =========================================================================
    // Sprite 0: 16x16 magenta square at (100, 70)
    obj_set_attr(&obj_buffer[0],
        ATTR0_SQUARE | ATTR0_4BPP,      // Square shape, 4bpp color
        ATTR1_SIZE_16,                   // 16x16 pixels
        ATTR2_PALBANK(0) | 0);           // Palette 0, tile 0
    obj_set_pos(&obj_buffer[0], 100, 70);

    // Sprite 1: 8x8 yellow square at (50, 50)
    obj_set_attr(&obj_buffer[1],
        ATTR0_SQUARE | ATTR0_4BPP,
        ATTR1_SIZE_8,
        ATTR2_PALBANK(0) | 4);           // Palette 0, tile 4
    obj_set_pos(&obj_buffer[1], 50, 50);

    // Copy to hardware OAM
    oam_copy(oam_mem, obj_buffer, 128);

    // =========================================================================
    // GAME LOOP
    // =========================================================================
    int x = 100, y = 70;

    while (1) {
        VBlankIntrWait();
        key_poll();

        // Move with D-pad
        x += key_tri_horz() * 2;  // Returns -1, 0, or 1
        y += key_tri_vert() * 2;

        // Clamp to screen
        x = clamp(x, 0, 240 - 16);
        y = clamp(y, 0, 160 - 16);

        // Update position
        obj_set_pos(&obj_buffer[0], x, y);

        // Copy OAM
        oam_copy(oam_mem, obj_buffer, 128);
    }

    return 0;
}
