/**
 * Meta Hunter - GBA Roguelite Game
 * enemy.h - Enemy definitions and AI
 */

#ifndef ENEMY_H
#define ENEMY_H

#include "game.h"

// =============================================================================
// ENEMY CONSTANTS
// =============================================================================
#define ENEMY_WIDTH         16
#define ENEMY_HEIGHT        16

// =============================================================================
// ENEMY TYPES
// =============================================================================
typedef enum {
    ENEMY_NONE = 0,
    ENEMY_WORM,         // Fast, low HP, chases player
    ENEMY_TROJAN,       // Slow, high HP, patrols
    ENEMY_TURRET        // Stationary, shoots at player
} EnemyType;

// =============================================================================
// ENEMY AI STATES
// =============================================================================
typedef enum {
    AI_IDLE,
    AI_PATROL,
    AI_CHASE,
    AI_ATTACK
} AIState;

// =============================================================================
// ENEMY STRUCTURE
// =============================================================================
typedef struct {
    // Position (fixed point)
    fixed8 x;
    fixed8 y;

    // Type and state
    EnemyType type;
    AIState ai_state;

    // Stats
    s16 hp;
    s16 max_hp;
    fixed8 speed;
    u8 damage;

    // AI behavior
    Direction move_dir;
    u8 state_timer;
    u8 attack_cooldown;

    // Patrol points (for patrol AI)
    int patrol_x1, patrol_y1;
    int patrol_x2, patrol_y2;
    BOOL patrol_forward;

    // Animation
    u8 anim_frame;
    u8 anim_timer;
    u8 hit_flash;

    // Active flag
    BOOL active;
} Enemy;

// Enemy pool
extern Enemy g_enemies[MAX_ENEMIES];

// =============================================================================
// ENEMY FUNCTIONS
// =============================================================================

// Initialize enemy system
void enemy_init_system(void);

// Spawn an enemy at position
int enemy_spawn(EnemyType type, int x, int y);

// Update all enemies
void enemy_update_all(void);

// Render all enemies
void enemy_render_all(void);

// Deal damage to an enemy (returns true if killed)
BOOL enemy_take_damage(int index, int amount);

// Kill an enemy
void enemy_kill(int index);

// Get enemy bounding box
void enemy_get_bounds(int index, int* left, int* top, int* right, int* bottom);

// Count active enemies
int enemy_count_active(void);

// Set up patrol path for an enemy
void enemy_set_patrol(int index, int x1, int y1, int x2, int y2);

#endif // ENEMY_H
