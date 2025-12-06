/**
 * Meta Hunter - GBA Roguelite Game
 * main.c - Entry point and main game loop
 */

#include <tonc.h>
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "combat.h"
#include "puzzle.h"
#include "ui.h"
#include "sprites.h"

// =============================================================================
// GLOBAL GAME CONTEXT
// =============================================================================
GameContext g_game;

// =============================================================================
// VBLANK HANDLER
// =============================================================================
void vblank_handler(void) {
    // Copy OAM buffer to hardware OAM
    oam_copy(oam_mem, obj_buffer, 128);
}

// =============================================================================
// GAME INITIALIZATION
// =============================================================================
void game_init(void) {
    // Initialize interrupts
    irq_init(NULL);
    irq_add(II_VBLANK, vblank_handler);

    // Set display mode: Mode 0 with BG0 for tiles, OBJ enabled
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    // Initialize OAM
    oam_init(obj_buffer, 128);

    // Load sprite palette and tiles
    sprites_init();

    // Initialize subsystems
    ui_init();
    projectile_init_system();
    enemy_init_system();
    puzzle_init_system();

    // Initialize game context
    g_game.state = STATE_TITLE;
    g_game.frame_count = 0;
    g_game.data_fragments = 0;
    g_game.current_room = 0;
    g_game.rooms_cleared = 0;

    // Seed random number generator with initial frame count
    puzzle_seed_random(0x12345678);
}

// =============================================================================
// START NEW GAME
// =============================================================================
static void start_new_game(void) {
    // Reset game state
    g_game.data_fragments = 0;
    g_game.current_room = 0;
    g_game.rooms_cleared = 0;

    // Initialize player in center of room
    player_init(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    // Clear any existing enemies and projectiles
    enemy_init_system();
    projectile_init_system();

    // Spawn some test enemies
    enemy_spawn(ENEMY_WORM, 50, 50);
    enemy_spawn(ENEMY_TROJAN, 180, 100);
    enemy_set_patrol(1, 150, 80, 200, 120);

    // Switch to playing state
    game_set_state(STATE_PLAYING);
}

// =============================================================================
// STATE TRANSITIONS
// =============================================================================
void game_set_state(GameState new_state) {
    GameState old_state = g_game.state;
    g_game.state = new_state;

    // Handle state exit
    switch (old_state) {
        case STATE_PUZZLE:
            // Re-enable sprites after puzzle
            REG_DISPCNT |= DCNT_OBJ;
            break;
        default:
            break;
    }

    // Handle state entry
    switch (new_state) {
        case STATE_TITLE:
            // Clear screen for title
            break;

        case STATE_PLAYING:
            // Normal gameplay mode
            REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
            break;

        case STATE_PUZZLE:
            // Start the puzzle
            puzzle_start();
            break;

        case STATE_GAMEOVER:
        case STATE_WIN:
            // Hide sprites during end screens
            break;

        default:
            break;
    }
}

// =============================================================================
// UPDATE TITLE SCREEN
// =============================================================================
static void update_title(void) {
    key_poll();

    // Seed RNG with frame count while waiting for input
    puzzle_seed_random(g_game.frame_count);

    // Start game on A or START
    if (key_hit(KEY_A) || key_hit(KEY_START)) {
        start_new_game();
    }
}

// =============================================================================
// UPDATE GAMEPLAY
// =============================================================================
static void update_playing(void) {
    key_poll();

    // Update player
    player_update();

    // Update enemies
    enemy_update_all();

    // Update projectiles
    projectile_update_all();

    // Check collisions
    collision_check_projectiles();
    collision_check_player_enemies();

    // Check for puzzle trigger (SELECT for testing)
    if (key_hit(KEY_SELECT)) {
        game_set_state(STATE_PUZZLE);
    }

    // Check for pause
    if (key_hit(KEY_START)) {
        game_set_state(STATE_PAUSED);
    }

    // Check win condition: all enemies dead
    if (enemy_count_active() == 0 && g_game.rooms_cleared == 0) {
        g_game.rooms_cleared++;
        // For prototype, trigger puzzle after clearing enemies
        game_set_state(STATE_PUZZLE);
    }

    // Check game over: player dead
    if (g_player.hp <= 0) {
        game_set_state(STATE_GAMEOVER);
    }
}

// =============================================================================
// UPDATE PUZZLE
// =============================================================================
static void update_puzzle(void) {
    key_poll();
    puzzle_update();

    // Check if puzzle is complete
    if (puzzle_is_complete()) {
        if (puzzle_was_won()) {
            // Award data fragments
            g_game.data_fragments += g_puzzle.data_reward;
            // Heal player a bit
            g_player.hp += g_puzzle.health_reward;
            if (g_player.hp > g_player.max_hp) {
                g_player.hp = g_player.max_hp;
            }
            // Check if this was the final objective
            if (g_game.rooms_cleared > 0) {
                game_set_state(STATE_WIN);
            } else {
                puzzle_end();
                game_set_state(STATE_PLAYING);
            }
        } else {
            // Puzzle failed - take damage or spawn enemies
            player_take_damage(20);
            if (g_player.hp <= 0) {
                game_set_state(STATE_GAMEOVER);
            } else {
                puzzle_end();
                game_set_state(STATE_PLAYING);
            }
        }
    }
}

// =============================================================================
// UPDATE PAUSED
// =============================================================================
static void update_paused(void) {
    key_poll();

    if (key_hit(KEY_START)) {
        game_set_state(STATE_PLAYING);
    }

    // Quit to title on SELECT
    if (key_hit(KEY_SELECT)) {
        game_set_state(STATE_TITLE);
    }
}

// =============================================================================
// UPDATE GAME OVER
// =============================================================================
static void update_gameover(void) {
    key_poll();

    // Return to title on any button
    if (key_hit(KEY_A) || key_hit(KEY_START)) {
        game_set_state(STATE_TITLE);
    }
}

// =============================================================================
// UPDATE WIN
// =============================================================================
static void update_win(void) {
    key_poll();

    // Return to title on any button
    if (key_hit(KEY_A) || key_hit(KEY_START)) {
        game_set_state(STATE_TITLE);
    }
}

// =============================================================================
// MAIN UPDATE
// =============================================================================
void game_update(void) {
    switch (g_game.state) {
        case STATE_TITLE:
            update_title();
            ui_render_title();
            break;

        case STATE_PLAYING:
            update_playing();
            player_render();
            enemy_render_all();
            projectile_render_all();
            ui_render_hud();
            break;

        case STATE_PUZZLE:
            update_puzzle();
            puzzle_render();
            break;

        case STATE_PAUSED:
            update_paused();
            ui_render_pause();
            break;

        case STATE_GAMEOVER:
            update_gameover();
            ui_render_gameover();
            break;

        case STATE_WIN:
            update_win();
            ui_render_win();
            break;
    }

    g_game.frame_count++;
}

// =============================================================================
// MAIN ENTRY POINT
// =============================================================================
int main(void) {
    // Initialize game
    game_init();

    // Main game loop
    while (1) {
        // Wait for VBlank (power efficient)
        VBlankIntrWait();

        // Update game logic and render
        game_update();
    }

    return 0;
}
