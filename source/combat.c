/**
 * Meta Hunter - GBA Roguelite Game
 * combat.c - Projectiles and collision detection
 */

#include <tonc.h>
#include "combat.h"
#include "player.h"
#include "enemy.h"
#include "sprites.h"

// =============================================================================
// PROJECTILE POOL
// =============================================================================
Projectile g_projectiles[MAX_PROJECTILES];

// =============================================================================
// PROJECTILE SYSTEM INIT
// =============================================================================
void projectile_init_system(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        g_projectiles[i].active = FALSE;

        // Hide OAM entry
        obj_hide(&obj_buffer[PROJECTILE_OAM_START + i]);
    }
}

// =============================================================================
// SPAWN PROJECTILE
// =============================================================================
int projectile_spawn(int x, int y, Direction dir, ProjectileOwner owner, int damage) {
    // Find free slot
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!g_projectiles[i].active) {
            Projectile* p = &g_projectiles[i];

            p->active = TRUE;
            p->x = INT_TO_FX8(x);
            p->y = INT_TO_FX8(y);
            p->owner = owner;
            p->damage = damage;
            p->lifetime = 120; // 2 seconds at 60fps

            // Set velocity based on direction
            p->vx = 0;
            p->vy = 0;

            switch (dir) {
                case DIR_UP:
                    p->vy = -PROJECTILE_SPEED;
                    break;
                case DIR_DOWN:
                    p->vy = PROJECTILE_SPEED;
                    break;
                case DIR_LEFT:
                    p->vx = -PROJECTILE_SPEED;
                    break;
                case DIR_RIGHT:
                    p->vx = PROJECTILE_SPEED;
                    break;
                default:
                    // No direction, despawn immediately
                    p->active = FALSE;
                    return -1;
            }

            return i;
        }
    }
    return -1; // No free slot
}

// =============================================================================
// UPDATE ALL PROJECTILES
// =============================================================================
void projectile_update_all(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile* p = &g_projectiles[i];
        if (!p->active) continue;

        // Move projectile
        p->x += p->vx;
        p->y += p->vy;

        // Check bounds
        int px = FX8_TO_INT(p->x);
        int py = FX8_TO_INT(p->y);

        if (px < WALL_LEFT || px > WALL_RIGHT ||
            py < WALL_TOP || py > WALL_BOTTOM) {
            projectile_despawn(i);
            continue;
        }

        // Decrease lifetime
        p->lifetime--;
        if (p->lifetime == 0) {
            projectile_despawn(i);
        }
    }
}

// =============================================================================
// RENDER ALL PROJECTILES
// =============================================================================
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

        // Determine tile and palette based on owner
        u16 tile_base = SPRITE_PROJECTILE_PLAYER;
        u8 palette = 0; // Player palette

        if (p->owner == PROJ_OWNER_ENEMY) {
            tile_base = SPRITE_PROJECTILE_ENEMY;
            palette = 1; // Enemy palette
        }

        // Set attributes: 8x8 sprite
        obj->attr0 = ATTR0_Y(y) | ATTR0_SQUARE | ATTR0_4BPP | ATTR0_REG;
        obj->attr1 = ATTR1_X(x) | ATTR1_SIZE_8;
        obj->attr2 = ATTR2_ID(tile_base) | ATTR2_PALBANK(palette) | ATTR2_PRIO(0);
    }
}

// =============================================================================
// DESPAWN PROJECTILE
// =============================================================================
void projectile_despawn(int index) {
    if (index < 0 || index >= MAX_PROJECTILES) return;

    g_projectiles[index].active = FALSE;
    obj_hide(&obj_buffer[PROJECTILE_OAM_START + index]);
}

// =============================================================================
// AABB COLLISION CHECK
// =============================================================================
BOOL collision_aabb(int ax1, int ay1, int ax2, int ay2,
                    int bx1, int by1, int bx2, int by2) {
    // Check for overlap
    return (ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1);
}

// =============================================================================
// CHECK PROJECTILE COLLISIONS
// =============================================================================
void collision_check_projectiles(void) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile* p = &g_projectiles[i];
        if (!p->active) continue;

        int px1 = FX8_TO_INT(p->x);
        int py1 = FX8_TO_INT(p->y);
        int px2 = px1 + PROJECTILE_WIDTH;
        int py2 = py1 + PROJECTILE_HEIGHT;

        if (p->owner == PROJ_OWNER_PLAYER) {
            // Check collision with enemies
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (!g_enemies[j].active) continue;

                int ex1, ey1, ex2, ey2;
                enemy_get_bounds(j, &ex1, &ey1, &ex2, &ey2);

                if (collision_aabb(px1, py1, px2, py2, ex1, ey1, ex2, ey2)) {
                    // Hit enemy
                    enemy_take_damage(j, p->damage);
                    projectile_despawn(i);
                    break;
                }
            }
        } else {
            // Enemy projectile - check collision with player
            if (!g_player.active) continue;

            int plx1, ply1, plx2, ply2;
            player_get_bounds(&plx1, &ply1, &plx2, &ply2);

            if (collision_aabb(px1, py1, px2, py2, plx1, ply1, plx2, ply2)) {
                // Hit player
                player_take_damage(p->damage);
                projectile_despawn(i);
            }
        }
    }
}

// =============================================================================
// CHECK PLAYER-ENEMY COLLISIONS (contact damage)
// =============================================================================
void collision_check_player_enemies(void) {
    if (!g_player.active) return;

    int plx1, ply1, plx2, ply2;
    player_get_bounds(&plx1, &ply1, &plx2, &ply2);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!g_enemies[i].active) continue;

        // Skip turrets for contact damage
        if (g_enemies[i].type == ENEMY_TURRET) continue;

        int ex1, ey1, ex2, ey2;
        enemy_get_bounds(i, &ex1, &ey1, &ex2, &ey2);

        if (collision_aabb(plx1, ply1, plx2, ply2, ex1, ey1, ex2, ey2)) {
            // Contact damage
            player_take_damage(g_enemies[i].damage);
            break; // Only take damage from one enemy per frame
        }
    }
}

// =============================================================================
// CHECK BOUNDS
// =============================================================================
BOOL collision_in_bounds(int x, int y, int width, int height) {
    return (x >= WALL_LEFT && x + width <= WALL_RIGHT &&
            y >= WALL_TOP && y + height <= WALL_BOTTOM);
}
