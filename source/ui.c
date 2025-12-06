/**
 * Meta Hunter - GBA Roguelite Game
 * ui.c - HUD and menu rendering
 *
 * Note: This uses Mode 3 (bitmap mode) for puzzle screens and text.
 * For gameplay, we use Mode 0 (tile mode) with hardware sprites.
 * A more complete implementation would use TTE (Tonc Text Engine)
 * or a custom tile-based text system.
 */

#include <tonc.h>
#include "ui.h"
#include "player.h"
#include "game.h"

// =============================================================================
// SIMPLE BITMAP FONT (3x5 digits only for prototype)
// =============================================================================
// Each digit is 3 pixels wide, 5 pixels tall
// Stored as 5 rows of 3 bits each
static const u8 digit_font[10][5] = {
    { 0b111, 0b101, 0b101, 0b101, 0b111 }, // 0
    { 0b010, 0b110, 0b010, 0b010, 0b111 }, // 1
    { 0b111, 0b001, 0b111, 0b100, 0b111 }, // 2
    { 0b111, 0b001, 0b111, 0b001, 0b111 }, // 3
    { 0b101, 0b101, 0b111, 0b001, 0b001 }, // 4
    { 0b111, 0b100, 0b111, 0b001, 0b111 }, // 5
    { 0b111, 0b100, 0b111, 0b101, 0b111 }, // 6
    { 0b111, 0b001, 0b010, 0b010, 0b010 }, // 7
    { 0b111, 0b101, 0b111, 0b101, 0b111 }, // 8
    { 0b111, 0b101, 0b111, 0b001, 0b111 }, // 9
};

// Simple ASCII characters (uppercase only, space, and some symbols)
// 4 pixels wide for most chars
static const u8 char_width = 4;
static const u8 char_height = 5;

// =============================================================================
// UI INITIALIZATION
// =============================================================================
void ui_init(void) {
    // Nothing special needed for prototype
}

// =============================================================================
// UI UPDATE
// =============================================================================
void ui_update(void) {
    // Nothing to update for prototype
}

// =============================================================================
// CLEAR SCREEN (Mode 3 bitmap)
// =============================================================================
void ui_clear_screen(u16 color) {
    // Switch to Mode 3 for bitmap drawing
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

    // Fill screen with color
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        vid_mem[i] = color;
    }
}

// =============================================================================
// DRAW RECTANGLE (Mode 3 bitmap)
// =============================================================================
void ui_draw_rect(int x, int y, int width, int height, u16 color) {
    // Clip to screen
    if (x < 0) { width += x; x = 0; }
    if (y < 0) { height += y; y = 0; }
    if (x + width > SCREEN_WIDTH) width = SCREEN_WIDTH - x;
    if (y + height > SCREEN_HEIGHT) height = SCREEN_HEIGHT - y;

    if (width <= 0 || height <= 0) return;

    for (int row = y; row < y + height; row++) {
        for (int col = x; col < x + width; col++) {
            vid_mem[row * SCREEN_WIDTH + col] = color;
        }
    }
}

// =============================================================================
// DRAW PROGRESS BAR
// =============================================================================
void ui_draw_bar(int x, int y, int width, int height,
                 int current, int max, u16 fill_color, u16 bg_color) {
    // Draw background
    ui_draw_rect(x, y, width, height, bg_color);

    // Calculate fill width
    int fill_width = 0;
    if (max > 0) {
        fill_width = (current * width) / max;
        if (fill_width < 0) fill_width = 0;
        if (fill_width > width) fill_width = width;
    }

    // Draw fill
    if (fill_width > 0) {
        ui_draw_rect(x, y, fill_width, height, fill_color);
    }

    // Draw border (1 pixel)
    // Top and bottom
    for (int col = x; col < x + width; col++) {
        vid_mem[y * SCREEN_WIDTH + col] = COLOR_WHITE;
        vid_mem[(y + height - 1) * SCREEN_WIDTH + col] = COLOR_WHITE;
    }
    // Left and right
    for (int row = y; row < y + height; row++) {
        vid_mem[row * SCREEN_WIDTH + x] = COLOR_WHITE;
        vid_mem[row * SCREEN_WIDTH + x + width - 1] = COLOR_WHITE;
    }
}

// =============================================================================
// DRAW SINGLE DIGIT
// =============================================================================
static void draw_digit(int x, int y, int digit, u16 color) {
    if (digit < 0 || digit > 9) return;

    for (int row = 0; row < 5; row++) {
        u8 bits = digit_font[digit][row];
        for (int col = 0; col < 3; col++) {
            if (bits & (0b100 >> col)) {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                    vid_mem[py * SCREEN_WIDTH + px] = color;
                }
            }
        }
    }
}

// =============================================================================
// DRAW NUMBER
// =============================================================================
void ui_draw_number(int x, int y, int number, int digits) {
    // Handle negative numbers
    if (number < 0) {
        // Draw minus sign
        ui_draw_rect(x, y + 2, 3, 1, COLOR_WHITE);
        x += 4;
        number = -number;
    }

    // Calculate digit positions (right to left)
    int digit_values[8];
    int actual_digits = 0;

    if (number == 0) {
        digit_values[0] = 0;
        actual_digits = 1;
    } else {
        int temp = number;
        while (temp > 0 && actual_digits < 8) {
            digit_values[actual_digits++] = temp % 10;
            temp /= 10;
        }
    }

    // Pad with zeros if needed
    while (actual_digits < digits) {
        digit_values[actual_digits++] = 0;
    }

    // Draw digits (left to right)
    for (int i = actual_digits - 1; i >= 0; i--) {
        draw_digit(x, y, digit_values[i], COLOR_WHITE);
        x += 4;
    }
}

// =============================================================================
// DRAW TEXT (Simple uppercase ASCII)
// =============================================================================
void ui_draw_text(int x, int y, const char* text) {
    int start_x = x;

    while (*text) {
        char c = *text++;

        // Handle newline
        if (c == '\n') {
            x = start_x;
            y += 7;
            continue;
        }

        // Handle space
        if (c == ' ') {
            x += 4;
            continue;
        }

        // Numbers
        if (c >= '0' && c <= '9') {
            draw_digit(x, y, c - '0', COLOR_WHITE);
            x += 4;
            continue;
        }

        // Convert to uppercase if lowercase
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }

        // Simple letters (very basic font - just filled rectangles as placeholders)
        if (c >= 'A' && c <= 'Z') {
            // Draw a simple 3x5 block for each letter
            // In a real implementation, you'd have proper character bitmaps
            for (int row = 0; row < 5; row++) {
                for (int col = 0; col < 3; col++) {
                    // Simple pattern - make each letter slightly different
                    BOOL draw = FALSE;
                    int letter = c - 'A';

                    // Create simple distinguishable patterns
                    if (row == 0 || row == 4) draw = TRUE;  // Top and bottom
                    if (col == 0) draw = TRUE;               // Left side always
                    if (row == 2 && letter != 2) draw = TRUE; // Middle (except C)
                    if (col == 2 && (row != 2 || letter > 12)) draw = TRUE; // Right varies

                    if (draw) {
                        int px = x + col;
                        int py = y + row;
                        if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                            vid_mem[py * SCREEN_WIDTH + px] = COLOR_WHITE;
                        }
                    }
                }
            }
            x += 4;
            continue;
        }

        // Colon
        if (c == ':') {
            vid_mem[(y + 1) * SCREEN_WIDTH + x] = COLOR_WHITE;
            vid_mem[(y + 3) * SCREEN_WIDTH + x] = COLOR_WHITE;
            x += 2;
            continue;
        }

        // Other characters - just advance
        x += 4;
    }
}

// =============================================================================
// RENDER HUD (during gameplay)
// =============================================================================
void ui_render_hud(void) {
    // Note: In a proper implementation, the HUD would use tile-based
    // backgrounds (Mode 0) rather than bitmap mode.
    // For now, we skip HUD rendering during gameplay to avoid mode switching.
    // The player can track their status visually through the sprites.

    // TODO: Implement proper tile-based HUD
}

// =============================================================================
// RENDER TITLE SCREEN
// =============================================================================
void ui_render_title(void) {
    ui_clear_screen(COLOR_BLACK);

    // Title
    ui_draw_text(60, 40, "META HUNTER");

    // Subtitle
    ui_draw_text(40, 60, "CODE CRACKER ROGUELITE");

    // Instructions
    ui_draw_text(60, 100, "PRESS START");

    // Version
    ui_draw_text(80, 140, "PHASE 1 PROTOTYPE");
}

// =============================================================================
// RENDER GAME OVER SCREEN
// =============================================================================
void ui_render_gameover(void) {
    ui_clear_screen(COLOR_BLACK);

    // Game over text
    ui_draw_rect(40, 50, 160, 60, COLOR_RED);
    ui_draw_text(80, 65, "GAME OVER");

    // Show data collected
    ui_draw_text(60, 90, "DATA COLLECTED:");
    ui_draw_number(170, 90, g_game.data_fragments, 4);

    // Instructions
    ui_draw_text(50, 130, "PRESS A TO CONTINUE");
}

// =============================================================================
// RENDER WIN SCREEN
// =============================================================================
void ui_render_win(void) {
    ui_clear_screen(COLOR_BLACK);

    // Victory text
    ui_draw_rect(40, 50, 160, 60, COLOR_GREEN);
    ui_draw_text(70, 65, "SYSTEM CRACKED");

    // Show data collected
    ui_draw_text(60, 90, "DATA COLLECTED:");
    ui_draw_number(170, 90, g_game.data_fragments, 4);

    // Instructions
    ui_draw_text(50, 130, "PRESS A TO CONTINUE");
}

// =============================================================================
// RENDER PAUSE MENU
// =============================================================================
void ui_render_pause(void) {
    ui_clear_screen(COLOR_DARK_GRAY);

    ui_draw_text(90, 60, "PAUSED");

    ui_draw_text(50, 90, "START: RESUME");
    ui_draw_text(50, 105, "SELECT: QUIT");

    // Show current stats
    ui_draw_text(50, 130, "HP:");
    ui_draw_number(80, 130, g_player.hp, 3);

    ui_draw_text(50, 140, "DATA:");
    ui_draw_number(95, 140, g_game.data_fragments, 4);
}
