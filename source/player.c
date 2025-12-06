/**
 * Meta Hunter - GBA Roguelite Game
 * player.c - Player character implementation
 */

#include <tonc.h>
#include "player.h"
#include "combat.h"
#include "sprites.h"

// =============================================================================
// GLOBAL PLAYER INSTANCE
// =============================================================================
Player g_player;

// =============================================================================
// PLAYER INITIALIZATION
// =============================================================================
void player_init(int start_x, int start_y) {
    g_player.x = INT_TO_FX8(start_x);
    g_player.y = INT_TO_FX8(start_y);

    g_player.hp = PLAYER_MAX_HP;
    g_player.shield = 0;
    g_player.max_hp = PLAYER_MAX_HP;
    g_player.max_shield = PLAYER_MAX_SHIELD;

    g_player.speed = PLAYER_SPEED;
    g_player.facing = DIR_RIGHT;

    g_player.shoot_cooldown = 0;
    g_player.invincibility_frames = 0;

    g_player.anim_frame = 0;
    g_player.anim_timer = 0;

    g_player.active = TRUE;
}

// =============================================================================
// PLAYER UPDATE
// =============================================================================
void player_update(void) {
    if (!g_player.active) return;

    // Handle movement input
    fixed8 move_x = 0;
    fixed8 move_y = 0;

    if (key_is_down(KEY_LEFT)) {
        move_x = -g_player.speed;
        g_player.facing = DIR_LEFT;
    }
    if (key_is_down(KEY_RIGHT)) {
        move_x = g_player.speed;
        g_player.facing = DIR_RIGHT;
    }
    if (key_is_down(KEY_UP)) {
        move_y = -g_player.speed;
        g_player.facing = DIR_UP;
    }
    if (key_is_down(KEY_DOWN)) {
        move_y = g_player.speed;
        g_player.facing = DIR_DOWN;
    }

    // Apply movement with boundary checking
    fixed8 new_x = g_player.x + move_x;
    fixed8 new_y = g_player.y + move_y;

    // Check horizontal bounds
    int px = FX8_TO_INT(new_x);
    if (px >= WALL_LEFT && px + PLAYER_WIDTH <= WALL_RIGHT) {
        g_player.x = new_x;
    }

    // Check vertical bounds
    int py = FX8_TO_INT(new_y);
    if (py >= WALL_TOP && py + PLAYER_HEIGHT <= WALL_BOTTOM) {
        g_player.y = new_y;
    }

    // Handle shooting
    if (g_player.shoot_cooldown > 0) {
        g_player.shoot_cooldown--;
    }

    if (key_is_down(KEY_A) && player_can_shoot()) {
        player_shoot();
    }

    // Update invincibility frames
    if (g_player.invincibility_frames > 0) {
        g_player.invincibility_frames--;
    }
}

// =============================================================================
// PLAYER RENDER
// =============================================================================
void player_render(void) {
    if (!g_player.active) {
        obj_hide(&obj_buffer[PLAYER_OAM_INDEX]);
        return;
    }

    int x = FX8_TO_INT(g_player.x);
    int y = FX8_TO_INT(g_player.y);

    // Blink when invincible
    if (g_player.invincibility_frames > 0 && (g_player.invincibility_frames & 0x04)) {
        obj_hide(&obj_buffer[PLAYER_OAM_INDEX]);
        return;
    }

    OBJ_ATTR* obj = &obj_buffer[PLAYER_OAM_INDEX];

    // Set attributes for 16x16 4bpp sprite
    obj->attr0 = ATTR0_Y(y) | ATTR0_SQUARE | ATTR0_4BPP;
    obj->attr1 = ATTR1_X(x) | ATTR1_SIZE_16;

    // Flip horizontally if facing left
    if (g_player.facing == DIR_LEFT) {
        obj->attr1 |= ATTR1_HFLIP;
    }

    // Use tile 0 for player sprite, palette 0
    obj->attr2 = ATTR2_ID(SPRITE_PLAYER_BASE) | ATTR2_PALBANK(0) | ATTR2_PRIO(1);
}

// =============================================================================
// PLAYER DAMAGE
// =============================================================================
BOOL player_take_damage(int amount) {
    if (g_player.invincibility_frames > 0) {
        return FALSE;
    }

    // Apply damage to shield first
    if (g_player.shield > 0) {
        if (g_player.shield >= amount) {
            g_player.shield -= amount;
            amount = 0;
        } else {
            amount -= g_player.shield;
            g_player.shield = 0;
        }
    }

    // Apply remaining damage to HP
    g_player.hp -= amount;

    // Grant invincibility frames
    g_player.invincibility_frames = 60;

    return (g_player.hp <= 0);
}

// =============================================================================
// PLAYER SHOOTING
// =============================================================================
BOOL player_can_shoot(void) {
    return g_player.shoot_cooldown == 0;
}

void player_shoot(void) {
    if (!player_can_shoot()) return;

    int px = FX8_TO_INT(g_player.x) + PLAYER_WIDTH / 2 - PROJECTILE_WIDTH / 2;
    int py = FX8_TO_INT(g_player.y) + PLAYER_HEIGHT / 2 - PROJECTILE_HEIGHT / 2;

    projectile_spawn(px, py, g_player.facing, PROJ_OWNER_PLAYER, PROJECTILE_DAMAGE);
    g_player.shoot_cooldown = PLAYER_SHOOT_COOLDOWN;
}

// =============================================================================
// PLAYER BOUNDS
// =============================================================================
void player_get_bounds(int* left, int* top, int* right, int* bottom) {
    *left = FX8_TO_INT(g_player.x);
    *top = FX8_TO_INT(g_player.y);
    *right = *left + PLAYER_WIDTH;
    *bottom = *top + PLAYER_HEIGHT;
}
