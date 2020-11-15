/* date = September 18th 2020 7:22 pm */
/*

Training 3: Slide Puzzle

Description: A 4x4 board of numbered tiles has one missing space and is randomly set up. To win the game, the player must slide tiles over to put the tiles back in order.

Variants: Instead of numbers, you can have a scrambled picture cut up into 4x4 tiles.

*/

#define SLIDE_PUZZLE_TITLE "Slide Puzzle"

#define SLIDE_PUZZLE_BOARD_COUNT 4

enum slide_puzzle_mode {
    SlidePuzzleMode_Ready,
    SlidePuzzleMode_Generating,
    
    SlidePuzzleMode_Count,
};

struct slide_puzzle_assets {
    loaded_font primary_font;
    loaded_font generating_font;
    loaded_font tile_font;
};

struct slide_puzzle_tile {
    u8 id; // NOTE(diego): 0 equals empty tile.
    char content[4];
};

struct slide_puzzle_board {
    u8 empty_index;
    slide_puzzle_tile tiles[SLIDE_PUZZLE_BOARD_COUNT][SLIDE_PUZZLE_BOARD_COUNT];
    // NOTE(diego): Save ids in correct order.
    s8 solution[SLIDE_PUZZLE_BOARD_COUNT * SLIDE_PUZZLE_BOARD_COUNT];
};

struct slide_puzzle_gen {
    s8 number;
    real32 shuffle_t;
    real32 shuffle_target;
    
    real32 fill_t;
    real32 fill_target;
    
    s8 shuffle_index;
    s8 tile_index;
};

struct slide_puzzle_swap {
    s8 from_index;
    s8 to_index;
};

struct slide_puzzle_state {
    slide_puzzle_assets assets;
    slide_puzzle_board board;
    
    v2i dimensions;
    
    slide_puzzle_mode mode;
    
    real32 sliding_t;
    real32 sliding_target;
    real32 sliding_t_rate;
    
    slide_puzzle_gen generation;
    slide_puzzle_swap swap;
    
    game_mode game_mode;
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void generate_puzzle(slide_puzzle_state *state);
internal void init_puzzle(slide_puzzle_state *state);
internal void init_puzzle(slide_puzzle_state *state, b32 animated);

internal void update_generating(slide_puzzle_state *state);

internal void draw_board(slide_puzzle_state *state);
internal void draw_game_view(slide_puzzle_state *state);

internal void slide_puzzle_menu_art(app_state *state, v2 min, v2 max);
internal void slide_puzzle_game_restart(slide_puzzle_state *state);
internal void slide_puzzle_game_update_and_render(game_memory *memory, game_input *input);
