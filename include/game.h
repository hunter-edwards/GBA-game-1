/**
 * Meta Hunter - GBA Roguelite Game
 * game.h - Main game definitions and constants
 */

#ifndef GAME_H
#define GAME_H

#include <tonc.h>

// =============================================================================
// DISPLAY CONSTANTS
// =============================================================================
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   160

// =============================================================================
// GAME STATES
// =============================================================================
typedef enum {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_PUZZLE,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_WIN
} GameState;

// =============================================================================
// ENTITY LIMITS
// =============================================================================
#define MAX_ENEMIES         8
#define MAX_PROJECTILES     16
#define PLAYER_OAM_INDEX    0
#define ENEMY_OAM_START     1
#define PROJECTILE_OAM_START (ENEMY_OAM_START + MAX_ENEMIES)

// =============================================================================
// ROOM CONSTANTS
// =============================================================================
#define ROOM_WIDTH          240
#define ROOM_HEIGHT         160
#define TILE_SIZE           8
#define ROOM_TILES_X        (ROOM_WIDTH / TILE_SIZE)
#define ROOM_TILES_Y        (ROOM_HEIGHT / TILE_SIZE)

// Wall boundaries (in pixels, accounting for HUD)
#define WALL_LEFT           8
#define WALL_RIGHT          (SCREEN_WIDTH - 8)
#define WALL_TOP            16  // Leave space for HUD
#define WALL_BOTTOM         (SCREEN_HEIGHT - 8)

// =============================================================================
// FIXED POINT MATH (8.8 format)
// =============================================================================
typedef s16 fixed8;

#define FX8_SHIFT       8
#define FX8_ONE         (1 << FX8_SHIFT)
#define INT_TO_FX8(i)   ((fixed8)((i) << FX8_SHIFT))
#define FX8_TO_INT(f)   ((f) >> FX8_SHIFT)
#define FX8_MUL(a, b)   ((fixed8)(((s32)(a) * (s32)(b)) >> FX8_SHIFT))

// =============================================================================
// DIRECTION ENUM
// =============================================================================
typedef enum {
    DIR_NONE = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

// =============================================================================
// GAME CONTEXT - Global game state
// =============================================================================
typedef struct {
    GameState state;
    u32 frame_count;
    u16 data_fragments;     // Currency
    u8  current_room;
    u8  rooms_cleared;
} GameContext;

// Global game context
extern GameContext g_game;

// =============================================================================
// CORE FUNCTIONS
// =============================================================================

// Initialization
void game_init(void);

// Main update (called every frame)
void game_update(void);

// State transitions
void game_set_state(GameState new_state);

// VBlank callback
void vblank_handler(void);

#endif // GAME_H
