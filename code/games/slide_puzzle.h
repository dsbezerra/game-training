/* date = September 18th 2020 7:22 pm */
/*

Training 3: Slide Puzzle

Description: A board full of overturned cards. There is a pair for each card. The player flips over two cards. If they match, then they stay overturned. Otherwise they flip back. The player needs to overturn all the cards in the fewest moves to win.

Variations: Provide "hints" in the form of four possible matching cards after the player flips the first one. Or, quickly overturn groups of cards at the beginning of the game.

*/

#define SLIDE_PUZZLE_TITLE "Slide Puzzle"

#define SLIDE_PUZZLE_BOARD_COUNT 4

struct slide_puzzle_assets {
    loaded_font primary_font;
    loaded_font tile_font;
};

struct slide_puzzle_tile {
    s8 id; // NOTE(diego): 0 equals empty tile.
    char content[4];
};

struct slide_puzzle_board {
    slide_puzzle_tile tiles[SLIDE_PUZZLE_BOARD_COUNT][SLIDE_PUZZLE_BOARD_COUNT];
    // NOTE(diego): Save ids in correct order.
    s8 solution[SLIDE_PUZZLE_BOARD_COUNT][SLIDE_PUZZLE_BOARD_COUNT];
};

struct slide_puzzle_state {
    slide_puzzle_assets assets;
    slide_puzzle_board board;
    
    v2i dimensions;
    
    game_mode game_mode;
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void init_puzzle(slide_puzzle_state *state);

internal void draw_board(slide_puzzle_state *state);

internal void draw_game_view(slide_puzzle_state *state);
internal void slide_puzzle_game_update_and_render(game_memory *memory, game_input *input);
