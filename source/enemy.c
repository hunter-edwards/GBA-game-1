/**
 * Meta Hunter - GBA Roguelite Game
 * enemy.c - Enemy AI and management
 */

#include <tonc.h>
#include "enemy.h"
#include "player.h"
#include "combat.h"
#include "sprites.h"

Enemy g_enemies[MAX_ENEMIES];

void enemy_init_system(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        g_enemies[i].active = FALSE;
        obj_hide(&obj_buffer[ENEMY_OAM_START + i]);
    }
}

int enemy_spawn(EnemyType type, int x, int y) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!g_enemies[i].active) {
            Enemy* e = &g_enemies[i];
            e->active = TRUE;
            e->type = type;
            e->x = INT_TO_FX8(x);
            e->y = INT_TO_FX8(y);

            // Stats by type
            switch (type) {
                case ENEMY_WORM:
                    e->hp = e->max_hp = 30;
                    e->speed = INT_TO_FX8(1);
                    e->damage = 10;
                    break;
                case ENEMY_TROJAN:
                    e->hp = e->max_hp = 80;
                    e->speed = FX8_ONE / 2;  // 0.5
                    e->damage = 15;
                    break;
                default:
                    e->hp = e->max_hp = 50;
                    e->speed = 0;
                    e->damage = 20;
                    break;
            }

            e->ai_state = AI_IDLE;
            e->move_dir = DIR_NONE;
            e->hit_flash = 0;
            e->patrol_x1 = e->patrol_x2 = x;
            e->patrol_y1 = e->patrol_y2 = y;
            e->patrol_forward = TRUE;
            e->state_timer = 0;

            return i;
        }
    }
    return -1;
}

void enemy_set_patrol(int index, int x1, int y1, int x2, int y2) {
    if (index < 0 || index >= MAX_ENEMIES) return;
    g_enemies[index].patrol_x1 = x1;
    g_enemies[index].patrol_y1 = y1;
    g_enemies[index].patrol_x2 = x2;
    g_enemies[index].patrol_y2 = y2;
    g_enemies[index].ai_state = AI_PATROL;
}

static void ai_worm(Enemy* e) {
    // Chase player
    int ex = FX8_TO_INT(e->x);
    int ey = FX8_TO_INT(e->y);
    int px = FX8_TO_INT(g_player.x);
    int py = FX8_TO_INT(g_player.y);

    fixed8 dx = 0, dy = 0;
    if (px < ex) dx = -e->speed;
    else if (px > ex) dx = e->speed;
    if (py < ey) dy = -e->speed;
    else if (py > ey) dy = e->speed;

    int new_x = FX8_TO_INT(e->x + dx);
    int new_y = FX8_TO_INT(e->y + dy);

    if (new_x >= WALL_LEFT && new_x + ENEMY_WIDTH <= WALL_RIGHT)
        e->x += dx;
    if (new_y >= WALL_TOP && new_y + ENEMY_HEIGHT <= WALL_BOTTOM)
        e->y += dy;

    e->move_dir = (dx < 0) ? DIR_LEFT : (dx > 0) ? DIR_RIGHT : e->move_dir;
}

static void ai_trojan(Enemy* e) {
    // Patrol between two points
    int target_x = e->patrol_forward ? e->patrol_x2 : e->patrol_x1;
    int target_y = e->patrol_forward ? e->patrol_y2 : e->patrol_y1;
    int ex = FX8_TO_INT(e->x);
    int ey = FX8_TO_INT(e->y);

    int dist_x = target_x - ex;
    int dist_y = target_y - ey;

    // Reached target?
    if (dist_x >= -4 && dist_x <= 4 && dist_y >= -4 && dist_y <= 4) {
        e->patrol_forward = !e->patrol_forward;
        e->state_timer = 30;
        return;
    }

    if (e->state_timer > 0) {
        e->state_timer--;
        return;
    }

    fixed8 dx = 0, dy = 0;
    if (dist_x < 0) dx = -e->speed;
    else if (dist_x > 0) dx = e->speed;
    if (dist_y < 0) dy = -e->speed;
    else if (dist_y > 0) dy = e->speed;

    e->x += dx;
    e->y += dy;
    e->move_dir = (dx < 0) ? DIR_LEFT : (dx > 0) ? DIR_RIGHT : e->move_dir;
}

void enemy_update_all(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy* e = &g_enemies[i];
        if (!e->active) continue;

        switch (e->type) {
            case ENEMY_WORM: ai_worm(e); break;
            case ENEMY_TROJAN: ai_trojan(e); break;
            default: break;
        }

        if (e->hit_flash > 0) e->hit_flash--;
    }
}

void enemy_render_all(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy* e = &g_enemies[i];
        OBJ_ATTR* obj = &obj_buffer[ENEMY_OAM_START + i];

        if (!e->active) {
            obj_hide(obj);
            continue;
        }

        int x = FX8_TO_INT(e->x);
        int y = FX8_TO_INT(e->y);

        u16 tile = SPRITE_ENEMY_WORM;
        if (e->type == ENEMY_TROJAN) tile = SPRITE_ENEMY_TROJAN;

        u8 pal = 1;  // Enemy palette
        if (e->hit_flash > 0 && (e->hit_flash & 2)) pal = 2;  // Flash white

        u16 attr1 = ATTR1_SIZE_16;
        if (e->move_dir == DIR_LEFT) attr1 |= ATTR1_HFLIP;

        obj_set_attr(obj, ATTR0_SQUARE | ATTR0_4BPP, attr1, ATTR2_PALBANK(pal) | tile);
        obj_set_pos(obj, x, y);
    }
}

BOOL enemy_take_damage(int index, int amount) {
    if (index < 0 || index >= MAX_ENEMIES) return FALSE;
    Enemy* e = &g_enemies[index];
    if (!e->active) return FALSE;

    e->hp -= amount;
    e->hit_flash = 10;

    if (e->hp <= 0) {
        enemy_kill(index);
        return TRUE;
    }
    return FALSE;
}

void enemy_kill(int index) {
    if (index < 0 || index >= MAX_ENEMIES) return;
    g_enemies[index].active = FALSE;
    obj_hide(&obj_buffer[ENEMY_OAM_START + index]);

    extern GameContext g_game;
    g_game.data_fragments += 10;
}

void enemy_get_bounds(int index, int* left, int* top, int* right, int* bottom) {
    if (index < 0 || index >= MAX_ENEMIES) return;
    *left = FX8_TO_INT(g_enemies[index].x);
    *top = FX8_TO_INT(g_enemies[index].y);
    *right = *left + ENEMY_WIDTH;
    *bottom = *top + ENEMY_HEIGHT;
}

int enemy_count_active(void) {
    int count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (g_enemies[i].active) count++;
    }
    return count;
}
