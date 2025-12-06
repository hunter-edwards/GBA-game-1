/**
 * Meta Hunter - GBA Roguelite Game
 * puzzle.c - Mastermind-style code breaking puzzle
 */

#include <tonc.h>
#include "puzzle.h"
#include "ui.h"

// =============================================================================
// GLOBAL PUZZLE INSTANCE
// =============================================================================
Puzzle g_puzzle;

// =============================================================================
// RANDOM NUMBER GENERATOR (xorshift32)
// =============================================================================
static u32 rng_state = 1;

void puzzle_seed_random(u32 seed) {
    rng_state = seed ? seed : 1; // Ensure non-zero
}

u32 puzzle_random(void) {
    u32 x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}

// Get random number in range [0, max)
static u32 random_range(u32 max) {
    return puzzle_random() % max;
}

// =============================================================================
// PUZZLE SYSTEM INIT
// =============================================================================
void puzzle_init_system(void) {
    g_puzzle.state = PUZZLE_INACTIVE;
}

// =============================================================================
// START NEW PUZZLE
// =============================================================================
void puzzle_start(void) {
    g_puzzle.state = PUZZLE_ENTERING;

    // Generate random secret code
    for (int i = 0; i < CODE_LENGTH; i++) {
        g_puzzle.secret_code[i] = random_range(CODE_MAX_VALUE + 1);
    }

    // Clear guesses and feedback
    for (int i = 0; i < MAX_GUESSES; i++) {
        for (int j = 0; j < CODE_LENGTH; j++) {
            g_puzzle.guesses[i][j] = 0;
            g_puzzle.feedback[i][j] = FEEDBACK_NONE;
        }
    }

    // Clear current input
    for (int i = 0; i < CODE_LENGTH; i++) {
        g_puzzle.current_input[i] = 0;
    }

    // Reset state
    g_puzzle.current_guess = 0;
    g_puzzle.cursor_pos = 0;
    g_puzzle.time_remaining = PUZZLE_TIME_LIMIT;

    // Set rewards
    g_puzzle.data_reward = 50;
    g_puzzle.health_reward = 10;
}

// =============================================================================
// CALCULATE FEEDBACK FOR A GUESS
// =============================================================================
void puzzle_calculate_feedback(u8 guess_index) {
    if (guess_index >= MAX_GUESSES) return;

    u8* guess = g_puzzle.guesses[guess_index];
    PuzzleFeedback* feedback = g_puzzle.feedback[guess_index];

    // Track which secret digits have been matched
    BOOL secret_used[CODE_LENGTH] = { FALSE };
    BOOL guess_used[CODE_LENGTH] = { FALSE };

    // First pass: find exact matches (correct position)
    for (int i = 0; i < CODE_LENGTH; i++) {
        if (guess[i] == g_puzzle.secret_code[i]) {
            feedback[i] = FEEDBACK_CORRECT;
            secret_used[i] = TRUE;
            guess_used[i] = TRUE;
        }
    }

    // Second pass: find misplaced digits
    for (int i = 0; i < CODE_LENGTH; i++) {
        if (guess_used[i]) continue; // Already matched

        for (int j = 0; j < CODE_LENGTH; j++) {
            if (secret_used[j]) continue; // Already used

            if (guess[i] == g_puzzle.secret_code[j]) {
                feedback[i] = FEEDBACK_MISPLACED;
                secret_used[j] = TRUE;
                guess_used[i] = TRUE;
                break;
            }
        }

        // If no match found, mark as wrong
        if (!guess_used[i]) {
            feedback[i] = FEEDBACK_WRONG;
        }
    }
}

// =============================================================================
// SUBMIT CURRENT GUESS
// =============================================================================
void puzzle_submit_guess(void) {
    if (g_puzzle.state != PUZZLE_ENTERING) return;
    if (g_puzzle.current_guess >= MAX_GUESSES) return;

    // Copy current input to guess array
    for (int i = 0; i < CODE_LENGTH; i++) {
        g_puzzle.guesses[g_puzzle.current_guess][i] = g_puzzle.current_input[i];
    }

    // Calculate feedback
    puzzle_calculate_feedback(g_puzzle.current_guess);

    // Check for win
    BOOL won = TRUE;
    for (int i = 0; i < CODE_LENGTH; i++) {
        if (g_puzzle.feedback[g_puzzle.current_guess][i] != FEEDBACK_CORRECT) {
            won = FALSE;
            break;
        }
    }

    if (won) {
        g_puzzle.state = PUZZLE_WON;
        return;
    }

    // Move to next guess
    g_puzzle.current_guess++;

    // Check for loss (out of guesses)
    if (g_puzzle.current_guess >= MAX_GUESSES) {
        g_puzzle.state = PUZZLE_LOST;
        return;
    }

    // Reset cursor and input for next guess
    g_puzzle.cursor_pos = 0;
    for (int i = 0; i < CODE_LENGTH; i++) {
        g_puzzle.current_input[i] = 0;
    }
}

// =============================================================================
// UPDATE PUZZLE
// =============================================================================
void puzzle_update(void) {
    if (g_puzzle.state != PUZZLE_ENTERING) return;

    // Update timer
    g_puzzle.time_remaining--;
    if (g_puzzle.time_remaining <= 0) {
        g_puzzle.state = PUZZLE_LOST;
        return;
    }

    // Handle input
    // Move cursor left/right
    if (key_hit(KEY_LEFT)) {
        if (g_puzzle.cursor_pos > 0) {
            g_puzzle.cursor_pos--;
        }
    }
    if (key_hit(KEY_RIGHT)) {
        if (g_puzzle.cursor_pos < CODE_LENGTH - 1) {
            g_puzzle.cursor_pos++;
        }
    }

    // Increment/decrement digit
    if (key_hit(KEY_UP)) {
        g_puzzle.current_input[g_puzzle.cursor_pos]++;
        if (g_puzzle.current_input[g_puzzle.cursor_pos] > CODE_MAX_VALUE) {
            g_puzzle.current_input[g_puzzle.cursor_pos] = 0;
        }
    }
    if (key_hit(KEY_DOWN)) {
        if (g_puzzle.current_input[g_puzzle.cursor_pos] == 0) {
            g_puzzle.current_input[g_puzzle.cursor_pos] = CODE_MAX_VALUE;
        } else {
            g_puzzle.current_input[g_puzzle.cursor_pos]--;
        }
    }

    // Submit guess on A
    if (key_hit(KEY_A)) {
        puzzle_submit_guess();
    }

    // Cancel puzzle on B (for testing - takes damage)
    if (key_hit(KEY_B)) {
        g_puzzle.state = PUZZLE_LOST;
    }
}

// =============================================================================
// RENDER PUZZLE
// =============================================================================
void puzzle_render(void) {
    // Clear screen to dark background
    ui_clear_screen(COLOR_DARK_GRAY);

    // Title
    ui_draw_text(80, 8, "CRACK THE CODE");

    // Draw timer bar
    int timer_width = (g_puzzle.time_remaining * 200) / PUZZLE_TIME_LIMIT;
    u16 timer_color = COLOR_GREEN;
    if (g_puzzle.time_remaining < PUZZLE_TIME_LIMIT / 4) {
        timer_color = COLOR_RED;
    } else if (g_puzzle.time_remaining < PUZZLE_TIME_LIMIT / 2) {
        timer_color = COLOR_YELLOW;
    }
    ui_draw_bar(20, 20, 200, 8, timer_width, 200, timer_color, COLOR_GRAY);

    // Draw previous guesses and feedback
    int y_start = 35;
    for (int row = 0; row < g_puzzle.current_guess; row++) {
        int y = y_start + row * 18;

        // Draw guess digits
        for (int col = 0; col < CODE_LENGTH; col++) {
            int x = 60 + col * 30;

            // Background color based on feedback
            u16 bg_color = COLOR_GRAY;
            switch (g_puzzle.feedback[row][col]) {
                case FEEDBACK_CORRECT:
                    bg_color = COLOR_GREEN;
                    break;
                case FEEDBACK_MISPLACED:
                    bg_color = COLOR_YELLOW;
                    break;
                case FEEDBACK_WRONG:
                    bg_color = COLOR_DARK_GRAY;
                    break;
                default:
                    break;
            }

            ui_draw_rect(x, y, 24, 16, bg_color);
            ui_draw_number(x + 8, y + 4, g_puzzle.guesses[row][col], 1);
        }
    }

    // Draw current input row
    if (g_puzzle.state == PUZZLE_ENTERING && g_puzzle.current_guess < MAX_GUESSES) {
        int y = y_start + g_puzzle.current_guess * 18;

        for (int col = 0; col < CODE_LENGTH; col++) {
            int x = 60 + col * 30;

            // Highlight cursor position
            u16 bg_color = COLOR_GRAY;
            if (col == g_puzzle.cursor_pos) {
                bg_color = COLOR_CYAN;
            }

            ui_draw_rect(x, y, 24, 16, bg_color);
            ui_draw_number(x + 8, y + 4, g_puzzle.current_input[col], 1);
        }

        // Draw cursor indicator
        int cursor_x = 60 + g_puzzle.cursor_pos * 30 + 8;
        int cursor_y = y + 18;
        ui_draw_text(cursor_x, cursor_y, "^");
    }

    // Draw instructions
    ui_draw_text(20, 140, "UP/DOWN:Digit LEFT/RIGHT:Move A:Submit");

    // Draw result if puzzle is complete
    if (g_puzzle.state == PUZZLE_WON) {
        ui_draw_rect(50, 60, 140, 40, COLOR_GREEN);
        ui_draw_text(70, 70, "CODE CRACKED!");
        ui_draw_text(65, 85, "Press A to continue");
    } else if (g_puzzle.state == PUZZLE_LOST) {
        ui_draw_rect(50, 60, 140, 40, COLOR_RED);
        ui_draw_text(85, 70, "FAILED!");

        // Show the secret code
        ui_draw_text(60, 85, "Code was:");
        for (int i = 0; i < CODE_LENGTH; i++) {
            ui_draw_number(120 + i * 15, 85, g_puzzle.secret_code[i], 1);
        }
    }
}

// =============================================================================
// CHECK PUZZLE COMPLETION
// =============================================================================
BOOL puzzle_is_complete(void) {
    return (g_puzzle.state == PUZZLE_WON || g_puzzle.state == PUZZLE_LOST);
}

BOOL puzzle_was_won(void) {
    return (g_puzzle.state == PUZZLE_WON);
}

// =============================================================================
// END PUZZLE
// =============================================================================
void puzzle_end(void) {
    g_puzzle.state = PUZZLE_INACTIVE;
}
