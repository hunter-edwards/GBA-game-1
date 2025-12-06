/**
 * Meta Hunter - GBA Roguelite Game
 * ui.h - HUD and menu rendering
 */

#ifndef UI_H
#define UI_H

#include "game.h"

// =============================================================================
// UI CONSTANTS
// =============================================================================
#define HUD_HEIGHT          16      // Top bar height
#define HP_BAR_X            4
#define HP_BAR_Y            4
#define HP_BAR_WIDTH        60
#define HP_BAR_HEIGHT       6

#define SHIELD_BAR_X        4
#define SHIELD_BAR_Y        11
#define SHIELD_BAR_WIDTH    60
#define SHIELD_BAR_HEIGHT   4

#define DATA_COUNTER_X      180
#define DATA_COUNTER_Y      4

// =============================================================================
// UI COLORS (15-bit RGB)
// =============================================================================
#define COLOR_BLACK         RGB15(0, 0, 0)
#define COLOR_WHITE         RGB15(31, 31, 31)
#define COLOR_RED           RGB15(31, 0, 0)
#define COLOR_GREEN         RGB15(0, 31, 0)
#define COLOR_BLUE          RGB15(0, 0, 31)
#define COLOR_YELLOW        RGB15(31, 31, 0)
#define COLOR_CYAN          RGB15(0, 31, 31)
#define COLOR_GRAY          RGB15(16, 16, 16)
#define COLOR_DARK_GRAY     RGB15(8, 8, 8)
#define COLOR_HP_RED        RGB15(28, 4, 4)
#define COLOR_SHIELD_BLUE   RGB15(4, 12, 28)
#define COLOR_DATA_YELLOW   RGB15(28, 28, 4)

// =============================================================================
// UI FUNCTIONS
// =============================================================================

// Initialize UI system
void ui_init(void);

// Update UI
void ui_update(void);

// Render HUD (health, shield, data counter)
void ui_render_hud(void);

// Render title screen
void ui_render_title(void);

// Render game over screen
void ui_render_gameover(void);

// Render win screen
void ui_render_win(void);

// Render pause menu
void ui_render_pause(void);

// =============================================================================
// DRAWING HELPERS
// =============================================================================

// Draw a filled rectangle (Mode 3 bitmap - for menus/debug)
void ui_draw_rect(int x, int y, int width, int height, u16 color);

// Draw a progress bar
void ui_draw_bar(int x, int y, int width, int height,
                 int current, int max, u16 fill_color, u16 bg_color);

// Draw text at position (using TTE or simple bitmap font)
void ui_draw_text(int x, int y, const char* text);

// Draw a number at position
void ui_draw_number(int x, int y, int number, int digits);

// Clear screen
void ui_clear_screen(u16 color);

#endif // UI_H
