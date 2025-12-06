/**
 * Meta Hunter - GBA Roguelite Game
 * main.c - MINIMAL TEST VERSION
 */

#include <tonc.h>
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "combat.h"
#include "sprites.h"

// =============================================================================
// GLOBAL GAME CONTEXT
// =============================================================================
GameContext g_game;

// =============================================================================
// MAIN ENTRY POINT - MINIMAL SPRITE TEST
// =============================================================================
int main(void) {
    // Initialize interrupts
    irq_init(NULL);
    irq_enable(II_VBLANK);

    // Set display mode: Mode 0 with OBJ enabled, 1D mapping
    REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;

    // Initialize OAM buffer
    oam_init(obj_buffer, 128);

    // =========================================================================
    // SET UP A SIMPLE SPRITE DIRECTLY (bypass sprites_init for testing)
    // =========================================================================

    // Set palette color 1 to bright magenta (very visible)
    pal_obj_mem[0] = RGB15(0, 0, 0);      // Color 0: transparent
    pal_obj_mem[1] = RGB15(31, 0, 31);    // Color 1: bright magenta
    pal_obj_mem[2] = RGB15(31, 31, 0);    // Color 2: yellow
    pal_obj_mem[3] = RGB15(0, 31, 0);     // Color 3: green

    // Create a solid 8x8 tile at tile 0
    // In 4bpp, each row is 8 pixels, each pixel is 4 bits = 32 bits total
    // Fill with color index 1 (magenta)
    u32 solid_row = 0x11111111;  // 8 pixels of color 1

    // Write to OBJ tile memory (tile 0)
    u32* tile0 = (u32*)tile_mem_obj[0];
    for (int i = 0; i < 8; i++) {
        tile0[i] = solid_row;
    }

    // Also create tiles 1, 2, 3 for 16x16 sprite
    u32* tile1 = (u32*)&tile_mem_obj[0][1];
    u32* tile2 = (u32*)&tile_mem_obj[0][2];
    u32* tile3 = (u32*)&tile_mem_obj[0][3];
    for (int i = 0; i < 8; i++) {
        tile1[i] = solid_row;
        tile2[i] = solid_row;
        tile3[i] = solid_row;
    }

    // Set up OAM entry 0 as a 16x16 sprite at position (100, 70)
    OBJ_ATTR* sprite = &obj_buffer[0];
    sprite->attr0 = ATTR0_Y(70) | ATTR0_SQUARE | ATTR0_4BPP;
    sprite->attr1 = ATTR1_X(100) | ATTR1_SIZE_16;
    sprite->attr2 = ATTR2_ID(0) | ATTR2_PALBANK(0);

    // Also set up an 8x8 sprite at OAM entry 1 for comparison
    OBJ_ATTR* sprite2 = &obj_buffer[1];
    sprite2->attr0 = ATTR0_Y(50) | ATTR0_SQUARE | ATTR0_4BPP;
    sprite2->attr1 = ATTR1_X(50) | ATTR1_SIZE_8;
    sprite2->attr2 = ATTR2_ID(0) | ATTR2_PALBANK(0);

    // Copy OAM buffer to hardware OAM
    oam_copy(oam_mem, obj_buffer, 128);

    // =========================================================================
    // SIMPLE GAME LOOP
    // =========================================================================
    int x = 100;
    int y = 70;

    while (1) {
        VBlankIntrWait();
        key_poll();

        // Move sprite with D-pad
        if (key_is_down(KEY_LEFT) && x > 0) x -= 2;
        if (key_is_down(KEY_RIGHT) && x < 224) x += 2;
        if (key_is_down(KEY_UP) && y > 0) y -= 2;
        if (key_is_down(KEY_DOWN) && y < 144) y += 2;

        // Update sprite position
        obj_set_pos(&obj_buffer[0], x, y);

        // Copy OAM to hardware
        oam_copy(oam_mem, obj_buffer, 128);
    }

    return 0;
}

// Stub to satisfy linker
void game_set_state(GameState new_state) {
    g_game.state = new_state;
}
