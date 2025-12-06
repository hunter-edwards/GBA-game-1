/**
 * Meta Hunter - GBA Roguelite Game
 * main.c - Entry point and main game loop
 *
 * PHASE 1 PROTOTYPE - Simplified for testing
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
// INITIALIZE BACKGROUND (simple solid color)
// =============================================================================
static void init_background(void) {
    // Set up BG0 as a simple tiled background
    // Use charblock 0 for tiles, screenblock 31 for map
    REG_BG0CNT = BG_CBB(0) | BG_SBB(31) | BG_4BPP | BG_REG_32x32;

    // Create a simple solid tile (8x8 pixels, all one color)
    // In 4bpp mode, each tile is 32 bytes (8x8 pixels, 4 bits each)
    u32* tile = (u32*)&tile_mem[0][1];  // Tile 1 (tile 0 is transparent)
    for (int i = 0; i < 8; i++) {
        tile[i] = 0x11111111;  // All pixels use palette color 1
    }

    // Set BG palette
    pal_bg_mem[0] = RGB15(0, 0, 0);      // Color 0: Black (transparent)
    pal_bg_mem[1] = RGB15(2, 2, 6);      // Color 1: Dark blue (floor)

    // Fill the screen map with tile 1
    u16* map = (u16*)se_mem[31];
    for (int i = 0; i < 32 * 32; i++) {
        map[i] = 1;  // Use tile 1
    }

    // Create wall tiles (brighter color)
    u32* wall_tile = (u32*)&tile_mem[0][2];  // Tile 2 for walls
    for (int i = 0; i < 8; i++) {
        wall_tile[i] = 0x22222222;  // All pixels use palette color 2
    }
    pal_bg_mem[2] = RGB15(8, 8, 12);  // Lighter blue-gray for walls

    // Draw border walls
    for (int x = 0; x < 30; x++) {
        map[x] = 2;              // Top wall
        map[19 * 32 + x] = 2;    // Bottom wall
    }
    for (int y = 0; y < 20; y++) {
        map[y * 32] = 2;         // Left wall
        map[y * 32 + 29] = 2;    // Right wall
    }
}

// =============================================================================
// GAME INITIALIZATION
// =============================================================================
void game_init(void) {
    // Initialize interrupts
    irq_init(NULL);
    irq_enable(II_VBLANK);

    // Set display mode: Mode 0 with BG0 and sprites
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    // Initialize OAM (hide all sprites initially)
    oam_init(obj_buffer, 128);

    // Set up background
    init_background();

    // Load sprite palettes and tiles
    sprites_init();

    // Initialize subsystems
    projectile_init_system();
    enemy_init_system();

    // Initialize game context
    g_game.state = STATE_PLAYING;
    g_game.frame_count = 0;
    g_game.data_fragments = 0;
    g_game.current_room = 0;
    g_game.rooms_cleared = 0;

    // Initialize player in center of room
    player_init(SCREEN_WIDTH / 2 - 8, SCREEN_HEIGHT / 2 - 8);

    // Spawn test enemies
    enemy_spawn(ENEMY_WORM, 40, 40);
    enemy_spawn(ENEMY_WORM, 180, 40);
    enemy_spawn(ENEMY_TROJAN, 100, 120);
    enemy_set_patrol(2, 60, 100, 160, 130);
}

// =============================================================================
// RESTART GAME
// =============================================================================
static void restart_game(void) {
    g_game.data_fragments = 0;
    g_game.rooms_cleared = 0;
    g_game.state = STATE_PLAYING;

    // Reset player
    player_init(SCREEN_WIDTH / 2 - 8, SCREEN_HEIGHT / 2 - 8);

    // Reset enemies and projectiles
    enemy_init_system();
    projectile_init_system();

    // Spawn new enemies
    enemy_spawn(ENEMY_WORM, 40, 40);
    enemy_spawn(ENEMY_WORM, 180, 40);
    enemy_spawn(ENEMY_TROJAN, 100, 120);
    enemy_set_patrol(2, 60, 100, 160, 130);
}

// =============================================================================
// UPDATE GAMEPLAY
// =============================================================================
static void update_playing(void) {
    // Update player
    player_update();

    // Update enemies
    enemy_update_all();

    // Update projectiles
    projectile_update_all();

    // Check collisions
    collision_check_projectiles();
    collision_check_player_enemies();

    // Check win condition: all enemies dead
    if (enemy_count_active() == 0) {
        // Spawn more enemies for now (endless mode)
        g_game.rooms_cleared++;
        g_game.data_fragments += 100;

        // Spawn new wave
        enemy_spawn(ENEMY_WORM, 30 + (g_game.rooms_cleared * 10) % 100, 40);
        enemy_spawn(ENEMY_WORM, 180 - (g_game.rooms_cleared * 10) % 80, 50);
        if (g_game.rooms_cleared >= 2) {
            enemy_spawn(ENEMY_TROJAN, 120, 100);
        }
    }

    // Check game over: player dead
    if (g_player.hp <= 0) {
        g_game.state = STATE_GAMEOVER;
    }
}

// =============================================================================
// UPDATE GAME OVER
// =============================================================================
static void update_gameover(void) {
    // Flash screen red briefly, then restart on button press
    if (key_hit(KEY_A) || key_hit(KEY_START)) {
        restart_game();
    }
}

// =============================================================================
// MAIN ENTRY POINT
// =============================================================================
int main(void) {
    // Initialize game
    game_init();

    // Main game loop
    while (1) {
        // Wait for VBlank
        VBlankIntrWait();

        // Poll input
        key_poll();

        // Update based on state
        if (g_game.state == STATE_PLAYING) {
            update_playing();
        } else if (g_game.state == STATE_GAMEOVER) {
            update_gameover();
        }

        // Render sprites
        player_render();
        enemy_render_all();
        projectile_render_all();

        // Copy OAM buffer to hardware
        oam_copy(oam_mem, obj_buffer, 128);

        g_game.frame_count++;
    }

    return 0;
}

// Stub functions to satisfy linker (puzzle disabled for now)
void game_set_state(GameState new_state) {
    g_game.state = new_state;
}
