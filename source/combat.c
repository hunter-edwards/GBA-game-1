/**
 * Meta Hunter - GBA Roguelite Game
 * combat.c - Projectiles and collision detection
 */

#include <tonc.h>
#include "combat.h"
#include "player.h"
#include "enemy.h"
#include "sprites.h"

Projectile g_projectiles[MAX_PROJECTILES];

void projectile_init_system(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        g_projectiles[i].active = FALSE;
        obj_hide(&obj_buffer[PROJECTILE_OAM_START + i]);
    }
}

int projectile_spawn(int x, int y, Direction dir, ProjectileOwner owner, int damage) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!g_projectiles[i].active) {
            Projectile* p = &g_projectiles[i];
            p->active = TRUE;
            p->x = INT_TO_FX8(x);
            p->y = INT_TO_FX8(y);
            p->owner = owner;
            p->damage = damage;
            p->lifetime = 120;

            p->vx = 0;
            p->vy = 0;
            switch (dir) {
                case DIR_UP:    p->vy = -PROJECTILE_SPEED; break;
                case DIR_DOWN:  p->vy = PROJECTILE_SPEED; break;
                case DIR_LEFT:  p->vx = -PROJECTILE_SPEED; break;
                case DIR_RIGHT: p->vx = PROJECTILE_SPEED; break;
                default: p->active = FALSE; return -1;
            }
            return i;
        }
    }
    return -1;
}

void projectile_update_all(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile* p = &g_projectiles[i];
        if (!p->active) continue;

        p->x += p->vx;
        p->y += p->vy;

        int px = FX8_TO_INT(p->x);
        int py = FX8_TO_INT(p->y);

        // Out of bounds?
        if (px < WALL_LEFT || px > WALL_RIGHT || py < WALL_TOP || py > WALL_BOTTOM) {
            projectile_despawn(i);
            continue;
        }

        p->lifetime--;
        if (p->lifetime == 0) {
            projectile_despawn(i);
        }
    }
}

void projectile_render_all(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile* p = &g_projectiles[i];
        OBJ_ATTR* obj = &obj_buffer[PROJECTILE_OAM_START + i];

        if (!p->active) {
            obj_hide(obj);
            continue;
        }

        int x = FX8_TO_INT(p->x);
        int y = FX8_TO_INT(p->y);

        u16 tile = (p->owner == PROJ_OWNER_PLAYER) ? SPRITE_PROJECTILE_PLAYER : SPRITE_PROJECTILE_ENEMY;
        u8 pal = (p->owner == PROJ_OWNER_PLAYER) ? 0 : 1;

        obj_set_attr(obj, ATTR0_SQUARE | ATTR0_4BPP, ATTR1_SIZE_8, ATTR2_PALBANK(pal) | tile);
        obj_set_pos(obj, x, y);
    }
}

void projectile_despawn(int index) {
    if (index < 0 || index >= MAX_PROJECTILES) return;
    g_projectiles[index].active = FALSE;
    obj_hide(&obj_buffer[PROJECTILE_OAM_START + index]);
}

BOOL collision_aabb(int ax1, int ay1, int ax2, int ay2,
                    int bx1, int by1, int bx2, int by2) {
    return (ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1);
}

void collision_check_projectiles(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile* p = &g_projectiles[i];
        if (!p->active) continue;

        int px1 = FX8_TO_INT(p->x);
        int py1 = FX8_TO_INT(p->y);
        int px2 = px1 + PROJECTILE_WIDTH;
        int py2 = py1 + PROJECTILE_HEIGHT;

        if (p->owner == PROJ_OWNER_PLAYER) {
            // Hit enemies
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (!g_enemies[j].active) continue;

                int ex1, ey1, ex2, ey2;
                enemy_get_bounds(j, &ex1, &ey1, &ex2, &ey2);

                if (collision_aabb(px1, py1, px2, py2, ex1, ey1, ex2, ey2)) {
                    enemy_take_damage(j, p->damage);
                    projectile_despawn(i);
                    break;
                }
            }
        } else {
            // Hit player
            if (!g_player.active) continue;

            int plx1, ply1, plx2, ply2;
            player_get_bounds(&plx1, &ply1, &plx2, &ply2);

            if (collision_aabb(px1, py1, px2, py2, plx1, ply1, plx2, ply2)) {
                player_take_damage(p->damage);
                projectile_despawn(i);
            }
        }
    }
}

void collision_check_player_enemies(void) {
    if (!g_player.active) return;

    int plx1, ply1, plx2, ply2;
    player_get_bounds(&plx1, &ply1, &plx2, &ply2);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!g_enemies[i].active) continue;

        int ex1, ey1, ex2, ey2;
        enemy_get_bounds(i, &ex1, &ey1, &ex2, &ey2);

        if (collision_aabb(plx1, ply1, plx2, ply2, ex1, ey1, ex2, ey2)) {
            player_take_damage(g_enemies[i].damage);
            break;
        }
    }
}

BOOL collision_in_bounds(int x, int y, int width, int height) {
    return (x >= WALL_LEFT && x + width <= WALL_RIGHT &&
            y >= WALL_TOP && y + height <= WALL_BOTTOM);
}
