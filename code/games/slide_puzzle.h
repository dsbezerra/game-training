/* date = September 18th 2020 7:22 pm */
/*

Training 3: Slide Puzzle

Description: A 4x4 board of numbered tiles has one missing space and is randomly set up. To win the game, the player must slide tiles over to put the tiles back in order.

Variants: Instead of numbers, you can have a scrambled picture cut up into 4x4 tiles.

*/

#define SLIDE_PUZZLE_TITLE "Slide Puzzle"

#define SLIDE_PUZZLE_BOARD_COUNT 4

enum Slide_Puzzle_Mode {
    SlidePuzzleMode_Ready,
    SlidePuzzleMode_Generating,
    
    SlidePuzzleMode_Count,
};

struct Slide_Puzzle_Assets {
    Loaded_Font primary_font;
    Loaded_Font generating_font;
    Loaded_Font tile_font;
};

struct Slide_Puzzle_Tile {
    u8 id; // NOTE(diego): 0 equals empty tile.
    char content[4];
};

struct Slide_Puzzle_Board {
    u8 empty_index;
    Slide_Puzzle_Tile tiles[SLIDE_PUZZLE_BOARD_COUNT][SLIDE_PUZZLE_BOARD_COUNT];
    // NOTE(diego): Save ids in correct order.
    s8 solution[SLIDE_PUZZLE_BOARD_COUNT * SLIDE_PUZZLE_BOARD_COUNT];
};

struct Slide_Puzzle_Gen {
    s8 number;
    real32 shuffle_t;
    real32 shuffle_target;
    
    real32 fill_t;
    real32 fill_target;
    
    s8 shuffle_index;
    s8 tile_index;
};

struct Slide_Puzzle_Swap {
    s8 from_index;
    s8 to_index;
};

struct Slide_Puzzle_State {
    Game_Mode game_mode;
    Game_Memory *memory;
    
    Slide_Puzzle_Mode mode;
    Slide_Puzzle_Assets assets;
    Slide_Puzzle_Board board;
    Slide_Puzzle_Gen generation;
    Slide_Puzzle_Swap swap;
    
    Vector2i dimensions;
    
    real32 sliding_t;
    real32 sliding_target;
    real32 sliding_t_rate;
};

internal void generate_puzzle(Slide_Puzzle_State *state);
internal void init_puzzle(Slide_Puzzle_State *state);
internal void init_puzzle(Slide_Puzzle_State *state, b32 animated);

internal void update_generating(Slide_Puzzle_State *state);

internal void draw_board(Slide_Puzzle_State *state);
internal void draw_game_view(Slide_Puzzle_State *state);

internal void slide_puzzle_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void slide_puzzle_game_restart(Slide_Puzzle_State *state);
internal void slide_puzzle_game_update_and_render(Game_Memory *memory, Game_Input *input);
