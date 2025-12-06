/**
 * Meta Hunter - GBA Roguelite Game
 * player.c - Player character
 */

#include <tonc.h>
#include "player.h"
#include "combat.h"
#include "sprites.h"

Player g_player;

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
    g_player.active = TRUE;
}

void player_update(void) {
    if (!g_player.active) return;

    // Movement
    fixed8 dx = 0, dy = 0;

    if (key_is_down(KEY_LEFT))  { dx = -g_player.speed; g_player.facing = DIR_LEFT; }
    if (key_is_down(KEY_RIGHT)) { dx = g_player.speed;  g_player.facing = DIR_RIGHT; }
    if (key_is_down(KEY_UP))    { dy = -g_player.speed; g_player.facing = DIR_UP; }
    if (key_is_down(KEY_DOWN))  { dy = g_player.speed;  g_player.facing = DIR_DOWN; }

    // Apply with bounds check
    int new_x = FX8_TO_INT(g_player.x + dx);
    int new_y = FX8_TO_INT(g_player.y + dy);

    if (new_x >= WALL_LEFT && new_x + PLAYER_WIDTH <= WALL_RIGHT)
        g_player.x += dx;
    if (new_y >= WALL_TOP && new_y + PLAYER_HEIGHT <= WALL_BOTTOM)
        g_player.y += dy;

    // Shooting
    if (g_player.shoot_cooldown > 0)
        g_player.shoot_cooldown--;

    if (key_is_down(KEY_A) && g_player.shoot_cooldown == 0) {
        player_shoot();
    }

    // Invincibility
    if (g_player.invincibility_frames > 0)
        g_player.invincibility_frames--;
}

void player_render(void) {
    OBJ_ATTR* obj = &obj_buffer[PLAYER_OAM_INDEX];

    if (!g_player.active) {
        obj_hide(obj);
        return;
    }

    int x = FX8_TO_INT(g_player.x);
    int y = FX8_TO_INT(g_player.y);

    // Blink when invincible
    if (g_player.invincibility_frames > 0 && (g_player.invincibility_frames & 4)) {
        obj_hide(obj);
        return;
    }

    // Set sprite attributes using libtonc
    u16 attr1 = ATTR1_SIZE_16;
    if (g_player.facing == DIR_LEFT) {
        attr1 |= ATTR1_HFLIP;
    }

    obj_set_attr(obj,
        ATTR0_SQUARE | ATTR0_4BPP,
        attr1,
        ATTR2_PALBANK(0) | SPRITE_PLAYER_BASE);
    obj_set_pos(obj, x, y);
}

BOOL player_take_damage(int amount) {
    if (g_player.invincibility_frames > 0) return FALSE;

    // Shield absorbs first
    if (g_player.shield > 0) {
        int absorbed = (g_player.shield >= amount) ? amount : g_player.shield;
        g_player.shield -= absorbed;
        amount -= absorbed;
    }

    g_player.hp -= amount;
    g_player.invincibility_frames = 60;

    return (g_player.hp <= 0);
}

BOOL player_can_shoot(void) {
    return g_player.shoot_cooldown == 0;
}

void player_shoot(void) {
    int px = FX8_TO_INT(g_player.x) + PLAYER_WIDTH / 2 - PROJECTILE_WIDTH / 2;
    int py = FX8_TO_INT(g_player.y) + PLAYER_HEIGHT / 2 - PROJECTILE_HEIGHT / 2;

    projectile_spawn(px, py, g_player.facing, PROJ_OWNER_PLAYER, PROJECTILE_DAMAGE);
    g_player.shoot_cooldown = PLAYER_SHOOT_COOLDOWN;
}

void player_get_bounds(int* left, int* top, int* right, int* bottom) {
    *left = FX8_TO_INT(g_player.x);
    *top = FX8_TO_INT(g_player.y);
    *right = *left + PLAYER_WIDTH;
    *bottom = *top + PLAYER_HEIGHT;
}
