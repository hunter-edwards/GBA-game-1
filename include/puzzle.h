/**
 * Meta Hunter - GBA Roguelite Game
 * puzzle.h - Mastermind-style code breaking puzzle
 */

#ifndef PUZZLE_H
#define PUZZLE_H

#include "game.h"

// =============================================================================
// PUZZLE CONSTANTS
// =============================================================================
#define CODE_LENGTH         4       // 4-digit code
#define CODE_MAX_VALUE      9       // Digits 0-9
#define MAX_GUESSES         6       // Maximum attempts
#define PUZZLE_TIME_LIMIT   (60 * 60)  // 60 seconds at 60fps

// =============================================================================
// FEEDBACK TYPES
// =============================================================================
typedef enum {
    FEEDBACK_NONE = 0,      // Not yet entered
    FEEDBACK_WRONG,         // Number not in code (white/empty)
    FEEDBACK_MISPLACED,     // Correct number, wrong position (yellow)
    FEEDBACK_CORRECT        // Correct number and position (green)
} PuzzleFeedback;

// =============================================================================
// PUZZLE STATE
// =============================================================================
typedef enum {
    PUZZLE_INACTIVE,
    PUZZLE_ENTERING,        // Player entering guess
    PUZZLE_SHOWING_RESULT,  // Showing feedback
    PUZZLE_WON,
    PUZZLE_LOST
} PuzzleState;

// =============================================================================
// PUZZLE STRUCTURE
// =============================================================================
typedef struct {
    // State
    PuzzleState state;

    // The secret code to crack
    u8 secret_code[CODE_LENGTH];

    // Player's guesses and feedback
    u8 guesses[MAX_GUESSES][CODE_LENGTH];
    PuzzleFeedback feedback[MAX_GUESSES][CODE_LENGTH];
    u8 current_guess;       // Which guess we're on (0-5)
    u8 cursor_pos;          // Which digit cursor is on (0-3)

    // Current input
    u8 current_input[CODE_LENGTH];

    // Timer
    s32 time_remaining;

    // Rewards
    u16 data_reward;
    u8 health_reward;
} Puzzle;

// Global puzzle instance
extern Puzzle g_puzzle;

// =============================================================================
// PUZZLE FUNCTIONS
// =============================================================================

// Initialize puzzle system
void puzzle_init_system(void);

// Start a new puzzle with random code
void puzzle_start(void);

// Update puzzle (handles input, timer)
void puzzle_update(void);

// Render puzzle UI
void puzzle_render(void);

// Submit current guess
void puzzle_submit_guess(void);

// Calculate feedback for a guess
void puzzle_calculate_feedback(u8 guess_index);

// Check if puzzle is complete (won or lost)
BOOL puzzle_is_complete(void);

// Get puzzle result (call after puzzle_is_complete returns true)
BOOL puzzle_was_won(void);

// End puzzle and return to gameplay
void puzzle_end(void);

// Random number generation
u32 puzzle_random(void);
void puzzle_seed_random(u32 seed);

#endif // PUZZLE_H
