/**
 * Meta Hunter - GBA Roguelite Game
 * sprites.h - Sprite definitions and tile indices
 */

#ifndef SPRITES_H
#define SPRITES_H

#include <tonc.h>

// =============================================================================
// SPRITE TILE INDICES
// =============================================================================
// Each 16x16 sprite uses 4 tiles (2x2) in 4bpp mode
// Each 8x8 sprite uses 1 tile

// Player sprites (16x16)
#define SPRITE_PLAYER_BASE      0   // Facing right, frame 0
#define SPRITE_PLAYER_FRAME2    4   // Facing right, frame 1
#define SPRITE_PLAYER_UP        8   // Facing up
#define SPRITE_PLAYER_DOWN      12  // Facing down

// Enemy sprites (16x16)
#define SPRITE_ENEMY_WORM       16
#define SPRITE_ENEMY_TROJAN     24
#define SPRITE_ENEMY_TURRET     32

// Projectile sprites (8x8)
#define SPRITE_PROJECTILE_PLAYER    40
#define SPRITE_PROJECTILE_ENEMY     41

// =============================================================================
// OAM BUFFER
// =============================================================================
extern OBJ_ATTR obj_buffer[128];

// =============================================================================
// SPRITE FUNCTIONS
// =============================================================================

// Initialize sprite system (load palettes and placeholder tiles)
void sprites_init(void);

#endif // SPRITES_H
