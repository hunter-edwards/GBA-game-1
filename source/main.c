/**
 * Meta Hunter - GBA Roguelite Game
 * main.c - Full game with working sprites
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
// INITIALIZE BACKGROUND
// =============================================================================
static void init_background(void) {
    // Set up BG0
    REG_BG0CNT = BG_CBB(0) | BG_SBB(31) | BG_4BPP | BG_REG_32x32;

    // Create floor tile (tile 1)
    TILE floor_tile;
    for (int i = 0; i < 8; i++) {
        floor_tile.data[i] = 0x11111111;  // Color 1
    }
    tile_mem[0][1] = floor_tile;

    // Create wall tile (tile 2)
    TILE wall_tile;
    for (int i = 0; i < 8; i++) {
        wall_tile.data[i] = 0x22222222;  // Color 2
    }
    tile_mem[0][2] = wall_tile;

    // Set BG palette
    pal_bg_mem[0] = RGB15(0, 0, 0);      // Transparent
    pal_bg_mem[1] = RGB15(2, 2, 8);      // Dark blue floor
    pal_bg_mem[2] = RGB15(6, 6, 12);     // Lighter blue walls

    // Fill map with floor
    u16* map = (u16*)se_mem[31];
    for (int i = 0; i < 32 * 32; i++) {
        map[i] = 1;
    }

    // Draw border walls
    for (int x = 0; x < 30; x++) {
        map[x] = 2;                // Top
        map[19 * 32 + x] = 2;      // Bottom
    }
    for (int y = 0; y < 20; y++) {
        map[y * 32] = 2;           // Left
        map[y * 32 + 29] = 2;      // Right
    }
}

// =============================================================================
// GAME INITIALIZATION
// =============================================================================
void game_init(void) {
    irq_init(NULL);
    irq_enable(II_VBLANK);

    // Mode 0: BG0 + sprites
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    oam_init(obj_buffer, 128);
    init_background();
    sprites_init();

    projectile_init_system();
    enemy_init_system();

    g_game.state = STATE_PLAYING;
    g_game.frame_count = 0;
    g_game.data_fragments = 0;
    g_game.rooms_cleared = 0;

    // Start player in center
    player_init(112, 72);

    // Spawn initial enemies
    enemy_spawn(ENEMY_WORM, 40, 40);
    enemy_spawn(ENEMY_WORM, 180, 40);
    enemy_spawn(ENEMY_TROJAN, 100, 110);
    enemy_set_patrol(2, 60, 90, 160, 130);
}

// =============================================================================
// RESTART GAME
// =============================================================================
static void restart_game(void) {
    g_game.state = STATE_PLAYING;
    g_game.data_fragments = 0;
    g_game.rooms_cleared = 0;

    player_init(112, 72);
    enemy_init_system();
    projectile_init_system();

    enemy_spawn(ENEMY_WORM, 40, 40);
    enemy_spawn(ENEMY_WORM, 180, 40);
    enemy_spawn(ENEMY_TROJAN, 100, 110);
    enemy_set_patrol(2, 60, 90, 160, 130);
}

// =============================================================================
// MAIN
// =============================================================================
int main(void) {
    game_init();

    while (1) {
        VBlankIntrWait();
        key_poll();

        if (g_game.state == STATE_PLAYING) {
            // Update game logic
            player_update();
            enemy_update_all();
            projectile_update_all();

            collision_check_projectiles();
            collision_check_player_enemies();

            // Spawn new wave when all enemies dead
            if (enemy_count_active() == 0) {
                g_game.rooms_cleared++;
                g_game.data_fragments += 50;

                // Spawn harder wave
                enemy_spawn(ENEMY_WORM, 30 + (g_game.rooms_cleared * 20) % 100, 35);
                enemy_spawn(ENEMY_WORM, 180 - (g_game.rooms_cleared * 15) % 80, 45);
                if (g_game.rooms_cleared >= 2) {
                    enemy_spawn(ENEMY_TROJAN, 120, 100);
                }
            }

            // Check death
            if (g_player.hp <= 0) {
                g_game.state = STATE_GAMEOVER;
            }

        } else if (g_game.state == STATE_GAMEOVER) {
            // Press A or START to restart
            if (key_hit(KEY_A) || key_hit(KEY_START)) {
                restart_game();
            }
        }

        // Render
        player_render();
        enemy_render_all();
        projectile_render_all();

        oam_copy(oam_mem, obj_buffer, 128);
        g_game.frame_count++;
    }

    return 0;
}

void game_set_state(GameState new_state) {
    g_game.state = new_state;
}
