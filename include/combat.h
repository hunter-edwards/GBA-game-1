/**
 * Meta Hunter - GBA Roguelite Game
 * combat.h - Projectiles and collision detection
 */

#ifndef COMBAT_H
#define COMBAT_H

#include "game.h"

// =============================================================================
// PROJECTILE CONSTANTS
// =============================================================================
#define PROJECTILE_WIDTH    8
#define PROJECTILE_HEIGHT   8
#define PROJECTILE_SPEED    INT_TO_FX8(4)
#define PROJECTILE_DAMAGE   25

// =============================================================================
// PROJECTILE OWNER
// =============================================================================
typedef enum {
    PROJ_OWNER_PLAYER,
    PROJ_OWNER_ENEMY
} ProjectileOwner;

// =============================================================================
// PROJECTILE STRUCTURE
// =============================================================================
typedef struct {
    // Position (fixed point)
    fixed8 x;
    fixed8 y;

    // Velocity
    fixed8 vx;
    fixed8 vy;

    // Properties
    ProjectileOwner owner;
    u8 damage;
    u8 lifetime;        // Frames until auto-despawn

    // Active flag
    BOOL active;
} Projectile;

// Projectile pool
extern Projectile g_projectiles[MAX_PROJECTILES];

// =============================================================================
// PROJECTILE FUNCTIONS
// =============================================================================

// Initialize projectile system
void projectile_init_system(void);

// Spawn a projectile
int projectile_spawn(int x, int y, Direction dir, ProjectileOwner owner, int damage);

// Update all projectiles
void projectile_update_all(void);

// Render all projectiles
void projectile_render_all(void);

// Despawn a projectile
void projectile_despawn(int index);

// =============================================================================
// COLLISION FUNCTIONS
// =============================================================================

// AABB collision check
BOOL collision_aabb(int ax1, int ay1, int ax2, int ay2,
                    int bx1, int by1, int bx2, int by2);

// Check all projectile collisions
void collision_check_projectiles(void);

// Check player-enemy collisions
void collision_check_player_enemies(void);

// Check if position is within room bounds
BOOL collision_in_bounds(int x, int y, int width, int height);

#endif // COMBAT_H
