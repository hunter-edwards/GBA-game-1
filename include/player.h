/**
 * Meta Hunter - GBA Roguelite Game
 * player.h - Player character definitions
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

// =============================================================================
// PLAYER CONSTANTS
// =============================================================================
#define PLAYER_WIDTH        16
#define PLAYER_HEIGHT       16
#define PLAYER_SPEED        INT_TO_FX8(2)
#define PLAYER_MAX_HP       100
#define PLAYER_MAX_SHIELD   50
#define PLAYER_SHOOT_COOLDOWN   15  // frames

// =============================================================================
// PLAYER STRUCTURE
// =============================================================================
typedef struct {
    // Position (fixed point for smooth movement)
    fixed8 x;
    fixed8 y;

    // Stats
    s16 hp;
    s16 shield;
    s16 max_hp;
    s16 max_shield;

    // Movement
    fixed8 speed;
    Direction facing;

    // Combat
    u8 shoot_cooldown;
    u8 invincibility_frames;

    // Animation
    u8 anim_frame;
    u8 anim_timer;

    // Active flag
    BOOL active;
} Player;

// Global player instance
extern Player g_player;

// =============================================================================
// PLAYER FUNCTIONS
// =============================================================================

// Initialize player at starting position
void player_init(int start_x, int start_y);

// Update player (movement, input, etc.)
void player_update(void);

// Render player sprite
void player_render(void);

// Deal damage to player (returns true if player dies)
BOOL player_take_damage(int amount);

// Player shoots a projectile
void player_shoot(void);

// Check if player can shoot
BOOL player_can_shoot(void);

// Get player bounding box for collision
void player_get_bounds(int* left, int* top, int* right, int* bottom);

#endif // PLAYER_H
