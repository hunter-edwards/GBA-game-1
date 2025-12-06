/**
 * Meta Hunter - GBA Roguelite Game
 * enemy.c - Enemy AI and management
 */

#include <tonc.h>
#include "enemy.h"
#include "player.h"
#include "combat.h"
#include "sprites.h"

// =============================================================================
// ENEMY POOL
// =============================================================================
Enemy g_enemies[MAX_ENEMIES];

// =============================================================================
// ENEMY TYPE STATS
// =============================================================================
typedef struct {
    s16 hp;
    fixed8 speed;
    u8 damage;
} EnemyStats;

static const EnemyStats enemy_stats[] = {
    [ENEMY_NONE]   = { 0,  0, 0 },
    [ENEMY_WORM]   = { 30, INT_TO_FX8(1), 10 },      // Fast, weak
    [ENEMY_TROJAN] = { 80, INT_TO_FX8(0), 15 },      // Slow, tanky (uses fixed point fraction)
    [ENEMY_TURRET] = { 50, 0, 20 }                   // Stationary
};

// Trojan speed as a fraction (0.5)
#define TROJAN_SPEED (FX8_ONE / 2)

// =============================================================================
// ENEMY SYSTEM INIT
// =============================================================================
void enemy_init_system(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        g_enemies[i].active = FALSE;
        g_enemies[i].type = ENEMY_NONE;

        // Hide OAM entry
        obj_hide(&obj_buffer[ENEMY_OAM_START + i]);
    }
}

// =============================================================================
// SPAWN ENEMY
// =============================================================================
int enemy_spawn(EnemyType type, int x, int y) {
    // Find free slot
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!g_enemies[i].active) {
            Enemy* e = &g_enemies[i];

            e->active = TRUE;
            e->type = type;
            e->x = INT_TO_FX8(x);
            e->y = INT_TO_FX8(y);

            // Set stats from table
            e->hp = enemy_stats[type].hp;
            e->max_hp = enemy_stats[type].hp;
            e->speed = enemy_stats[type].speed;
            e->damage = enemy_stats[type].damage;

            // Special handling for Trojan fractional speed
            if (type == ENEMY_TROJAN) {
                e->speed = TROJAN_SPEED;
            }

            // Initialize AI
            e->ai_state = (type == ENEMY_TURRET) ? AI_ATTACK : AI_IDLE;
            e->move_dir = DIR_NONE;
            e->state_timer = 0;
            e->attack_cooldown = 0;

            // Default patrol (can be overridden)
            e->patrol_x1 = x;
            e->patrol_y1 = y;
            e->patrol_x2 = x;
            e->patrol_y2 = y;
            e->patrol_forward = TRUE;

            // Animation
            e->anim_frame = 0;
            e->anim_timer = 0;
            e->hit_flash = 0;

            return i;
        }
    }
    return -1; // No free slot
}

// =============================================================================
// SET PATROL PATH
// =============================================================================
void enemy_set_patrol(int index, int x1, int y1, int x2, int y2) {
    if (index < 0 || index >= MAX_ENEMIES || !g_enemies[index].active) return;

    g_enemies[index].patrol_x1 = x1;
    g_enemies[index].patrol_y1 = y1;
    g_enemies[index].patrol_x2 = x2;
    g_enemies[index].patrol_y2 = y2;
    g_enemies[index].ai_state = AI_PATROL;
}

// =============================================================================
// AI: WORM - Chases player
// =============================================================================
static void ai_worm(Enemy* e) {
    int ex = FX8_TO_INT(e->x);
    int ey = FX8_TO_INT(e->y);
    int px = FX8_TO_INT(g_player.x);
    int py = FX8_TO_INT(g_player.y);

    // Simple chase: move toward player
    fixed8 dx = 0, dy = 0;

    if (px < ex) dx = -e->speed;
    else if (px > ex) dx = e->speed;

    if (py < ey) dy = -e->speed;
    else if (py > ey) dy = e->speed;

    // Apply movement with bounds check
    fixed8 new_x = e->x + dx;
    fixed8 new_y = e->y + dy;

    int nx = FX8_TO_INT(new_x);
    int ny = FX8_TO_INT(new_y);

    if (nx >= WALL_LEFT && nx + ENEMY_WIDTH <= WALL_RIGHT) {
        e->x = new_x;
    }
    if (ny >= WALL_TOP && ny + ENEMY_HEIGHT <= WALL_BOTTOM) {
        e->y = new_y;
    }

    // Set facing direction
    if (dx < 0) e->move_dir = DIR_LEFT;
    else if (dx > 0) e->move_dir = DIR_RIGHT;
}

// =============================================================================
// AI: TROJAN - Patrols between two points
// =============================================================================
static void ai_trojan(Enemy* e) {
    // Get target based on patrol direction
    int target_x = e->patrol_forward ? e->patrol_x2 : e->patrol_x1;
    int target_y = e->patrol_forward ? e->patrol_y2 : e->patrol_y1;

    int ex = FX8_TO_INT(e->x);
    int ey = FX8_TO_INT(e->y);

    // Check if reached target
    int dist_x = target_x - ex;
    int dist_y = target_y - ey;

    if (dist_x >= -4 && dist_x <= 4 && dist_y >= -4 && dist_y <= 4) {
        // Reached target, reverse direction
        e->patrol_forward = !e->patrol_forward;
        e->state_timer = 30; // Pause briefly
        return;
    }

    // Pause at endpoints
    if (e->state_timer > 0) {
        e->state_timer--;
        return;
    }

    // Move toward target
    fixed8 dx = 0, dy = 0;

    if (dist_x < 0) dx = -e->speed;
    else if (dist_x > 0) dx = e->speed;

    if (dist_y < 0) dy = -e->speed;
    else if (dist_y > 0) dy = e->speed;

    e->x += dx;
    e->y += dy;

    // Set facing
    if (dx < 0) e->move_dir = DIR_LEFT;
    else if (dx > 0) e->move_dir = DIR_RIGHT;
}

// =============================================================================
// AI: TURRET - Stationary, shoots at player
// =============================================================================
static void ai_turret(Enemy* e) {
    // Decrease attack cooldown
    if (e->attack_cooldown > 0) {
        e->attack_cooldown--;
        return;
    }

    // Calculate direction to player
    int ex = FX8_TO_INT(e->x) + ENEMY_WIDTH / 2;
    int ey = FX8_TO_INT(e->y) + ENEMY_HEIGHT / 2;
    int px = FX8_TO_INT(g_player.x) + PLAYER_WIDTH / 2;
    int py = FX8_TO_INT(g_player.y) + PLAYER_HEIGHT / 2;

    int dx = px - ex;
    int dy = py - ey;

    // Determine primary direction
    Direction shoot_dir = DIR_RIGHT;
    if (dx < 0 && -dx > dy && -dx > -dy) shoot_dir = DIR_LEFT;
    else if (dx > 0 && dx > dy && dx > -dy) shoot_dir = DIR_RIGHT;
    else if (dy < 0) shoot_dir = DIR_UP;
    else shoot_dir = DIR_DOWN;

    // Shoot projectile
    int spawn_x = FX8_TO_INT(e->x) + ENEMY_WIDTH / 2 - PROJECTILE_WIDTH / 2;
    int spawn_y = FX8_TO_INT(e->y) + ENEMY_HEIGHT / 2 - PROJECTILE_HEIGHT / 2;

    projectile_spawn(spawn_x, spawn_y, shoot_dir, PROJ_OWNER_ENEMY, e->damage);

    // Set cooldown (slower than player)
    e->attack_cooldown = 90; // 1.5 seconds

    e->move_dir = shoot_dir;
}

// =============================================================================
// UPDATE ALL ENEMIES
// =============================================================================
void enemy_update_all(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy* e = &g_enemies[i];
        if (!e->active) continue;

        // Update AI based on type
        switch (e->type) {
            case ENEMY_WORM:
                ai_worm(e);
                break;
            case ENEMY_TROJAN:
                ai_trojan(e);
                break;
            case ENEMY_TURRET:
                ai_turret(e);
                break;
            default:
                break;
        }

        // Update hit flash
        if (e->hit_flash > 0) {
            e->hit_flash--;
        }

        // Update animation
        e->anim_timer++;
        if (e->anim_timer >= 10) {
            e->anim_timer = 0;
            e->anim_frame = (e->anim_frame + 1) % 2;
        }
    }
}

// =============================================================================
// RENDER ALL ENEMIES
// =============================================================================
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

        // Position
        obj_set_pos(obj, x, y);

        // Determine tile based on enemy type
        u16 tile_base;
        u8 palette;

        switch (e->type) {
            case ENEMY_WORM:
                tile_base = SPRITE_ENEMY_WORM;
                palette = 1; // Enemy palette
                break;
            case ENEMY_TROJAN:
                tile_base = SPRITE_ENEMY_TROJAN;
                palette = 1;
                break;
            case ENEMY_TURRET:
                tile_base = SPRITE_ENEMY_TURRET;
                palette = 1;
                break;
            default:
                tile_base = SPRITE_ENEMY_WORM;
                palette = 1;
                break;
        }

        // Use different palette when hit (flash white)
        if (e->hit_flash > 0 && (e->hit_flash & 0x02)) {
            palette = 2; // White flash palette
        }

        // Set attributes (no animation offset - use base tile only)
        obj->attr0 = ATTR0_Y(y) | ATTR0_SQUARE | ATTR0_4BPP;
        obj->attr1 = ATTR1_X(x) | ATTR1_SIZE_16;

        // Flip based on movement direction
        if (e->move_dir == DIR_LEFT) {
            obj->attr1 |= ATTR1_HFLIP;
        }

        obj->attr2 = ATTR2_ID(tile_base) | ATTR2_PALBANK(palette) | ATTR2_PRIO(1);
    }
}

// =============================================================================
// ENEMY DAMAGE
// =============================================================================
BOOL enemy_take_damage(int index, int amount) {
    if (index < 0 || index >= MAX_ENEMIES) return FALSE;

    Enemy* e = &g_enemies[index];
    if (!e->active) return FALSE;

    e->hp -= amount;
    e->hit_flash = 10; // Flash for 10 frames

    if (e->hp <= 0) {
        enemy_kill(index);
        return TRUE;
    }
    return FALSE;
}

// =============================================================================
// ENEMY DEATH
// =============================================================================
void enemy_kill(int index) {
    if (index < 0 || index >= MAX_ENEMIES) return;

    g_enemies[index].active = FALSE;
    g_enemies[index].type = ENEMY_NONE;

    // Hide OAM sprite
    obj_hide(&obj_buffer[ENEMY_OAM_START + index]);

    // Award data fragments (could vary by enemy type)
    extern GameContext g_game;
    g_game.data_fragments += 10;
}

// =============================================================================
// ENEMY BOUNDS
// =============================================================================
void enemy_get_bounds(int index, int* left, int* top, int* right, int* bottom) {
    if (index < 0 || index >= MAX_ENEMIES) return;

    *left = FX8_TO_INT(g_enemies[index].x);
    *top = FX8_TO_INT(g_enemies[index].y);
    *right = *left + ENEMY_WIDTH;
    *bottom = *top + ENEMY_HEIGHT;
}

// =============================================================================
// COUNT ACTIVE ENEMIES
// =============================================================================
int enemy_count_active(void) {
    int count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (g_enemies[i].active) count++;
    }
    return count;
}
